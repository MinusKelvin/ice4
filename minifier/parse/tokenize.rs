use super::*;
use Token::*;

pub fn tokenize(ast: Vec<TopLevel>) -> Vec<Token> {
    let mut result = vec![];
    for tl in ast {
        tl.tokenize(&mut result);
    }
    result
}

impl TopLevel {
    fn tokenize(self, result: &mut Vec<Token>) {
        match self {
            TopLevel::Declaration(d) => {
                d.tokenize(result);
                result.push(Semicolon);
            }
            TopLevel::Using(ns) => result.extend([
                Keyword("using"),
                Keyword("namespace"),
                Identifier(ns),
                Semicolon,
            ]),
            TopLevel::Function(f) => f.tokenize(result),
            TopLevel::Struct(name, members, decls) => {
                result.extend([Keyword("struct"), Typename(name), LeftBrace]);
                for item in members {
                    item.tokenize(result);
                }
                result.push(RightBrace);
                for declexpr in decls {
                    declexpr.tokenize(result);
                }
                result.push(Semicolon);
            }
        }
    }
}

impl Declaration {
    fn tokenize(self, result: &mut Vec<Token>) {
        self.base_type.tokenize(result);
        for (i, declexpr) in self.declarations.into_iter().enumerate() {
            if i != 0 {
                result.push(Comma);
            }
            declexpr.tokenize(result);
        }
    }
}

impl Function {
    fn tokenize(self, result: &mut Vec<Token>) {
        self.base_type.tokenize(result);
        self.decl_form.tokenize(result);
        result.push(LeftParen);
        for (i, (ty, decl)) in self.args.into_iter().enumerate() {
            if i != 0 {
                result.push(Comma);
            }
            ty.tokenize(result);
            decl.tokenize(result);
        }
        result.push(RightParen);
        block(self.body, result);
    }
}

impl BaseType {
    fn tokenize(self, result: &mut Vec<Token>) {
        if self.const_ {
            result.push(Keyword("const"));
        }
        match self.core {
            CoreType::User(name) => result.push(Typename(name)),
            CoreType::SignedChar => result.push(Keyword("char")),
            CoreType::SignedShort => result.push(Keyword("short")),
            CoreType::SignedInt => result.push(Keyword("int")),
            CoreType::SignedLong => result.push(Keyword("long")),
            CoreType::UnsignedChar => result.extend([Keyword("unsigned"), Keyword("char")]),
            CoreType::UnsignedShort => result.extend([Keyword("unsigned"), Keyword("short")]),
            CoreType::UnsignedInt => result.push(Keyword("unsigned")),
            CoreType::UnsignedLong => result.extend([Keyword("unsigned"), Keyword("long")]),
            CoreType::Float => result.push(Keyword("float")),
            CoreType::Double => result.push(Keyword("double")),
            CoreType::Void => result.push(Keyword("void")),
            CoreType::Auto => result.push(Keyword("auto")),
        }
        if let Some(template) = self.template_parameters {
            result.push(Less);
            for (i, ty) in template.into_iter().enumerate() {
                if i != 0 {
                    result.push(Comma);
                }
                ty.tokenize(result);
            }
            result.push(Greater);
        }
    }
}

impl ModifiedType {
    fn tokenize(self, result: &mut Vec<Token>) {
        match self {
            ModifiedType::Base(ty) => ty.tokenize(result),
            ModifiedType::Pointer(ty) => {
                ty.tokenize(result);
                result.push(Star);
            }
            ModifiedType::LReference(ty) => {
                ty.tokenize(result);
                result.push(Ampersand);
            }
            ModifiedType::RReference(ty) => {
                ty.tokenize(result);
                result.push(AmpersandAmpersand);
            }
        }
    }
}

impl DeclExpr {
    fn tokenize(self, result: &mut Vec<Token>) {
        self.form.tokenize(result);
        match self.init {
            Initializer::Default => {}
            Initializer::Call(args) => {
                result.push(LeftParen);
                for (i, arg) in args.into_iter().enumerate() {
                    if i != 0 {
                        result.push(Comma);
                    }
                    arg.tokenize(result, Precedence::Assignment);
                }
                result.push(RightParen);
            }
            Initializer::Brace(args) => {
                result.push(LeftBrace);
                for (i, arg) in args.into_iter().enumerate() {
                    if i != 0 {
                        result.push(Comma);
                    }
                    arg.tokenize(result, Precedence::Assignment);
                }
                result.push(RightBrace);
            }
            Initializer::Equal(v) => {
                result.push(Equal);
                v.tokenize(result, Precedence::Assignment);
            }
            Initializer::Array(args) => {
                result.push(Equal);
                result.push(LeftBrace);
                for (i, arg) in args.into_iter().enumerate() {
                    if i != 0 {
                        result.push(Comma);
                    }
                    arg.tokenize(result, Precedence::Assignment);
                }
                result.push(RightBrace);
            }
        }
    }
}

impl DeclForm {
    fn tokenize(self, result: &mut Vec<Token>) {
        match self {
            DeclForm::OpEquals => result.extend([Keyword("operator"), EqualEqual]),
            DeclForm::Name(n) => result.push(Identifier(n)),
            DeclForm::Pointer(form) => {
                result.push(Star);
                form.tokenize(result);
            }
            DeclForm::LReference(form) => {
                result.push(Ampersand);
                form.tokenize(result);
            }
            DeclForm::RReference(form) => {
                result.push(AmpersandAmpersand);
                form.tokenize(result);
            }
            DeclForm::Array(form, len) => {
                form.tokenize(result);
                result.push(LeftBracket);
                if let Some(l) = len {
                    result.push(number(l));
                }
                result.push(RightBracket);
            }
        }
    }
}

impl MemberItem {
    fn tokenize(self, result: &mut Vec<Token>) {
        match self {
            MemberItem::Field(d) => {
                d.tokenize(result);
                result.push(Semicolon);
            }
            MemberItem::Method(f) => f.tokenize(result),
            MemberItem::Constructor(name, args, inits, code) => {
                result.extend([Typename(name), LeftParen]);
                for (i, (ty, decl)) in args.into_iter().enumerate() {
                    if i != 0 {
                        result.push(Comma);
                    }
                    ty.tokenize(result);
                    decl.tokenize(result);
                }
                result.push(RightParen);
                if !inits.is_empty() {
                    result.push(Colon);
                    for (i, (name, init)) in inits.into_iter().enumerate() {
                        if i != 0 {
                            result.push(Comma);
                        }
                        result.push(Identifier(name));
                        let (l, args, r) = match init {
                            Initializer::Call(args) => (LeftParen, args, RightParen),
                            Initializer::Brace(args) => (LeftBrace, args, RightBrace),
                            _ => unreachable!(),
                        };
                        result.push(l);
                        for (i, expr) in args.into_iter().enumerate() {
                            if i != 0 {
                                result.push(Comma);
                            }
                            expr.tokenize(result, Precedence::Comma);
                        }
                        result.push(r);
                    }
                }
                block(code, result);
            }
            MemberItem::DefaultedConstructor(name) => result.extend([
                Typename(name),
                LeftParen,
                RightParen,
                Equal,
                Keyword("default"),
                Semicolon,
            ]),
        }
    }
}

#[derive(PartialEq, Eq, PartialOrd, Ord)]
enum Precedence {
    Comma,
    Assignment,
    LogicalOr,
    LogicalAnd,
    BitOr,
    BitXor,
    BitAnd,
    Equality,
    Compare,
    BitShift,
    Addition,
    Multiplication,
    Prefix,
    Postfix,
}

impl Expr {
    fn precedence(&self) -> Precedence {
        match self {
            Expr::Number(_) => Precedence::Postfix,
            Expr::String(_) => Precedence::Postfix,
            Expr::Ident(_) => Precedence::Postfix,
            Expr::Lambda(_, _, _) => Precedence::Postfix,
            Expr::Comma(_, _) => Precedence::Comma,
            Expr::Throw(_) => Precedence::Assignment,
            Expr::Assign(_, _) => Precedence::Assignment,
            Expr::OrAssign(_, _) => Precedence::Assignment,
            Expr::XorAssign(_, _) => Precedence::Assignment,
            Expr::AndAssign(_, _) => Precedence::Assignment,
            Expr::LeftShiftAssign(_, _) => Precedence::Assignment,
            Expr::RightShiftAssign(_, _) => Precedence::Assignment,
            Expr::AddAssign(_, _) => Precedence::Assignment,
            Expr::SubAssign(_, _) => Precedence::Assignment,
            Expr::MulAssign(_, _) => Precedence::Assignment,
            Expr::DivAssign(_, _) => Precedence::Assignment,
            Expr::ModAssign(_, _) => Precedence::Assignment,
            Expr::Ternary(_, _, _) => Precedence::Assignment,
            Expr::LogicalOr(_, _) => Precedence::LogicalOr,
            Expr::LogicalAnd(_, _) => Precedence::LogicalAnd,
            Expr::BitOr(_, _) => Precedence::BitOr,
            Expr::BitXor(_, _) => Precedence::BitXor,
            Expr::BitAnd(_, _) => Precedence::BitAnd,
            Expr::Equals(_, _) => Precedence::Equality,
            Expr::NotEquals(_, _) => Precedence::Equality,
            Expr::Less(_, _) => Precedence::Compare,
            Expr::LessEquals(_, _) => Precedence::Compare,
            Expr::Greater(_, _) => Precedence::Compare,
            Expr::GreaterEquals(_, _) => Precedence::Compare,
            Expr::ShiftLeft(_, _) => Precedence::BitShift,
            Expr::ShiftRight(_, _) => Precedence::BitShift,
            Expr::Add(_, _) => Precedence::Addition,
            Expr::Sub(_, _) => Precedence::Addition,
            Expr::Mul(_, _) => Precedence::Multiplication,
            Expr::Div(_, _) => Precedence::Multiplication,
            Expr::Mod(_, _) => Precedence::Multiplication,
            Expr::Deref(_) => Precedence::Prefix,
            Expr::AddressOf(_) => Precedence::Prefix,
            Expr::UnaryPlus(_) => Precedence::Prefix,
            Expr::Negate(_) => Precedence::Prefix,
            Expr::Not(_) => Precedence::Prefix,
            Expr::Complement(_) => Precedence::Prefix,
            Expr::PreIncrement(_) => Precedence::Prefix,
            Expr::PreDecrement(_) => Precedence::Prefix,
            Expr::SizeOf(_) => Precedence::Prefix,
            Expr::SizeOfType(_) => Precedence::Prefix,
            Expr::Cast(_, _) => Precedence::Prefix,
            Expr::Index(_, _) => Precedence::Postfix,
            Expr::Call(_, _) => Precedence::Postfix,
            Expr::Construct(_, _) => Precedence::Postfix,
            Expr::BraceConstruct(_, _) => Precedence::Postfix,
            Expr::MemberAccess(_, _) => Precedence::Postfix,
            Expr::PointerMemberAccess(_, _) => Precedence::Postfix,
            Expr::PostIncrement(_) => Precedence::Postfix,
            Expr::PostDecrement(_) => Precedence::Postfix,
        }
    }

    fn tokenize(self, result: &mut Vec<Token>, grouping_level: Precedence) {
        let prec = self.precedence();
        if prec < grouping_level {
            result.push(LeftParen);
        }
        match self {
            Expr::Number(n) => result.push(number(n)),
            Expr::String(s) => result.push(String(s)),
            Expr::Ident(n) => result.push(Identifier(n)),
            Expr::Lambda(caps, args, b) => {
                result.push(LeftBracket);
                for (i, cap) in caps.into_iter().enumerate() {
                    if i != 0 {
                        result.push(Comma);
                    }
                    result.push(Identifier(cap));
                }
                result.push(RightBracket);
                result.push(LeftParen);
                for (i, (ty, decl)) in args.into_iter().enumerate() {
                    if i != 0 {
                        result.push(Comma);
                    }
                    ty.tokenize(result);
                    decl.tokenize(result);
                }
                result.push(RightParen);
                block(b, result);
            }
            Expr::Comma(l, r) => {
                l.tokenize(result, Precedence::Comma);
                result.push(Comma);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::Throw(v) => {
                result.push(Keyword("throw"));
                v.tokenize(result, Precedence::Assignment);
            }
            Expr::Assign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(Equal);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::OrAssign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(BarEqual);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::XorAssign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(CaretEqual);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::AndAssign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(AmpersandEqual);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::LeftShiftAssign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(LessLessEqual);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::RightShiftAssign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(GreaterGreaterEqual);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::AddAssign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(PlusEqual);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::SubAssign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(MinusEqual);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::MulAssign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(StarEqual);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::DivAssign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(SlashEqual);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::ModAssign(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(PercentEqual);
                r.tokenize(result, Precedence::Assignment);
            }
            Expr::Ternary(c, t, f) => {
                c.tokenize(result, Precedence::LogicalOr);
                result.push(Question);
                t.tokenize(result, Precedence::Comma);
                result.push(Colon);
                f.tokenize(result, Precedence::Assignment);
            }
            Expr::LogicalOr(l, r) => {
                l.tokenize(result, Precedence::LogicalOr);
                result.push(BarBar);
                r.tokenize(result, Precedence::LogicalAnd);
            }
            Expr::LogicalAnd(l, r) => {
                l.tokenize(result, Precedence::LogicalAnd);
                result.push(AmpersandAmpersand);
                r.tokenize(result, Precedence::BitOr);
            }
            Expr::BitOr(l, r) => {
                l.tokenize(result, Precedence::BitOr);
                result.push(Bar);
                r.tokenize(result, Precedence::BitXor);
            }
            Expr::BitXor(l, r) => {
                l.tokenize(result, Precedence::BitXor);
                result.push(Caret);
                r.tokenize(result, Precedence::BitAnd);
            }
            Expr::BitAnd(l, r) => {
                l.tokenize(result, Precedence::BitAnd);
                result.push(Ampersand);
                r.tokenize(result, Precedence::Equality);
            }
            Expr::Equals(l, r) => {
                l.tokenize(result, Precedence::Equality);
                result.push(EqualEqual);
                r.tokenize(result, Precedence::Compare);
            }
            Expr::NotEquals(l, r) => {
                l.tokenize(result, Precedence::Equality);
                result.push(BangEqual);
                r.tokenize(result, Precedence::Compare);
            }
            Expr::Less(l, r) => {
                l.tokenize(result, Precedence::Compare);
                result.push(Less);
                r.tokenize(result, Precedence::BitShift);
            }
            Expr::LessEquals(l, r) => {
                l.tokenize(result, Precedence::Compare);
                result.push(LessEqual);
                r.tokenize(result, Precedence::BitShift);
            }
            Expr::Greater(l, r) => {
                l.tokenize(result, Precedence::Compare);
                result.push(Greater);
                r.tokenize(result, Precedence::BitShift);
            }
            Expr::GreaterEquals(l, r) => {
                l.tokenize(result, Precedence::Compare);
                result.push(GreaterEqual);
                r.tokenize(result, Precedence::BitShift);
            }
            Expr::ShiftLeft(l, r) => {
                l.tokenize(result, Precedence::BitShift);
                result.push(LessLess);
                r.tokenize(result, Precedence::Addition);
            }
            Expr::ShiftRight(l, r) => {
                l.tokenize(result, Precedence::BitShift);
                result.push(GreaterGreater);
                r.tokenize(result, Precedence::Addition);
            }
            Expr::Add(l, r) => {
                l.tokenize(result, Precedence::Addition);
                result.push(Plus);
                r.tokenize(result, Precedence::Multiplication);
            }
            Expr::Sub(l, r) => {
                l.tokenize(result, Precedence::Addition);
                result.push(Minus);
                r.tokenize(result, Precedence::Multiplication);
            }
            Expr::Mul(l, r) => {
                l.tokenize(result, Precedence::Multiplication);
                result.push(Star);
                r.tokenize(result, Precedence::Prefix);
            }
            Expr::Div(l, r) => {
                l.tokenize(result, Precedence::Multiplication);
                result.push(Slash);
                r.tokenize(result, Precedence::Prefix);
            }
            Expr::Mod(l, r) => {
                l.tokenize(result, Precedence::Multiplication);
                result.push(Percent);
                r.tokenize(result, Precedence::Prefix);
            }
            Expr::Deref(e) => {
                result.push(Star);
                e.tokenize(result, Precedence::Prefix);
            }
            Expr::AddressOf(e) => {
                result.push(Ampersand);
                e.tokenize(result, Precedence::Prefix);
            }
            Expr::UnaryPlus(e) => {
                result.push(Plus);
                e.tokenize(result, Precedence::Prefix);
            }
            Expr::Negate(e) => {
                result.push(Minus);
                e.tokenize(result, Precedence::Prefix);
            }
            Expr::Not(e) => {
                result.push(Bang);
                e.tokenize(result, Precedence::Prefix);
            }
            Expr::Complement(e) => {
                result.push(Tilde);
                e.tokenize(result, Precedence::Prefix);
            }
            Expr::PreIncrement(e) => {
                result.push(PlusPlus);
                e.tokenize(result, Precedence::Prefix);
            }
            Expr::PreDecrement(e) => {
                result.push(MinusMinus);
                e.tokenize(result, Precedence::Prefix);
            }
            Expr::SizeOf(e) => {
                result.push(Keyword("sizeof"));
                if matches!(*e, Expr::Cast(_, _)) {
                    result.push(LeftParen);
                    e.tokenize(result, Precedence::Comma);
                    result.push(RightParen);
                } else {
                    e.tokenize(result, Precedence::Prefix);
                }
            }
            Expr::SizeOfType(ty) => {
                result.extend([Keyword("sizeof"), LeftParen]);
                ty.tokenize(result);
                result.push(RightParen);
            }
            Expr::Cast(ty, e) => {
                result.push(LeftParen);
                ty.tokenize(result);
                result.push(RightParen);
                e.tokenize(result, Precedence::Prefix);
            }
            Expr::Index(e, i) => {
                e.tokenize(result, Precedence::Postfix);
                result.push(LeftBracket);
                i.tokenize(result, Precedence::Comma);
                result.push(RightBracket);
            }
            Expr::Call(e, args) => {
                e.tokenize(result, Precedence::Postfix);
                result.push(LeftParen);
                for (i, arg) in args.into_iter().enumerate() {
                    if i != 0 {
                        result.push(Comma);
                    }
                    arg.tokenize(result, Precedence::Assignment);
                }
                result.push(RightParen);
            }
            Expr::Construct(e, args) => {
                e.tokenize(result);
                result.push(LeftParen);
                for (i, arg) in args.into_iter().enumerate() {
                    if i != 0 {
                        result.push(Comma);
                    }
                    arg.tokenize(result, Precedence::Assignment);
                }
                result.push(RightParen);
            }
            Expr::BraceConstruct(e, args) => {
                e.tokenize(result);
                result.push(LeftBrace);
                for (i, arg) in args.into_iter().enumerate() {
                    if i != 0 {
                        result.push(Comma);
                    }
                    arg.tokenize(result, Precedence::Assignment);
                }
                result.push(RightBrace);
            }
            Expr::MemberAccess(e, name) => {
                e.tokenize(result, Precedence::Postfix);
                result.extend([Dot, Identifier(name)]);
            }
            Expr::PointerMemberAccess(e, name) => {
                e.tokenize(result, Precedence::Postfix);
                result.extend([MinusGreater, Identifier(name)]);
            }
            Expr::PostIncrement(e) => {
                e.tokenize(result, Precedence::Postfix);
                result.push(PlusPlus);
            }
            Expr::PostDecrement(e) => {
                e.tokenize(result, Precedence::Postfix);
                result.push(MinusMinus);
            }
        }
        if prec < grouping_level {
            result.push(RightParen);
        }
    }
}

impl Statement {
    fn catches_else(&self) -> bool {
        match self {
            Statement::Declaration(_) => false,
            Statement::Expression(_) => false,
            Statement::Case(_) => false,
            Statement::Try(_) => false,
            Statement::For(_, _, _, b) => b.len() == 1 && b.last().unwrap().catches_else(),
            Statement::ForEach(_, _, b) => b.len() == 1 && b.last().unwrap().catches_else(),
            Statement::Return(_) => false,
            Statement::If(_, _, f) => f.is_empty(),
            Statement::Switch(_, _) => false,
            Statement::While(_, b) => b.len() == 1 && b.last().unwrap().catches_else(),
            Statement::Continue => false,
            Statement::Break => false,
        }
    }

    fn tokenize(self, result: &mut Vec<Token>) {
        match self {
            Statement::Declaration(d) => {
                d.tokenize(result);
                result.push(Semicolon);
            }
            Statement::Expression(e) => {
                e.tokenize(result, Precedence::Comma);
                result.push(Semicolon);
            }
            Statement::Case(v) => {
                result.push(Keyword("case"));
                v.tokenize(result, Precedence::Comma);
                result.push(Colon);
            }
            Statement::Try(code) => {
                result.push(Keyword("try"));
                block(code, result);
                result.extend([
                    Keyword("catch"),
                    LeftParen,
                    DotDotDot,
                    RightParen,
                    LeftBrace,
                    RightBrace,
                ])
            }
            Statement::For(d, c, i, b) => {
                result.extend([Keyword("for"), LeftParen]);
                if let Some(d) = d {
                    d.tokenize(result);
                }
                result.push(Semicolon);
                if let Some(c) = c {
                    c.tokenize(result, Precedence::Comma);
                }
                result.push(Semicolon);
                if let Some(i) = i {
                    i.tokenize(result, Precedence::Comma);
                }
                result.push(RightParen);
                statement_block(b, result);
            }
            Statement::ForEach((ty, decl), iter, b) => {
                result.extend([Keyword("for"), LeftParen]);
                ty.tokenize(result);
                decl.tokenize(result);
                result.push(Colon);
                match iter {
                    Ok(e) => e.tokenize(result, Precedence::Comma),
                    Err(vs) => {
                        result.push(LeftBrace);
                        for (i, v) in vs.into_iter().enumerate() {
                            if i != 0 {
                                result.push(Comma);
                            }
                            v.tokenize(result, Precedence::Assignment);
                        }
                        result.push(RightBrace);
                    },
                }
                result.push(RightParen);
                statement_block(b, result);
            }
            Statement::Return(e) => {
                result.push(Keyword("return"));
                if let Some(e) = e {
                    e.tokenize(result, Precedence::Comma);
                }
                result.push(Semicolon);
            }
            Statement::If(c, t, f) => {
                result.extend([Keyword("if"), LeftParen]);
                c.tokenize(result, Precedence::Comma);
                result.push(RightParen);
                if t.is_empty() {
                    result.push(Semicolon);
                } else if t.len() > 1 || !f.is_empty() && t.last().unwrap().catches_else() {
                    block(t, result);
                } else {
                    t.into_iter().next().unwrap().tokenize(result);
                }
                if !f.is_empty() {
                    result.push(Keyword("else"));
                    statement_block(f, result);
                }
            }
            Statement::Switch(v, b) => {
                result.extend([Keyword("switch"), LeftParen]);
                v.tokenize(result, Precedence::Comma);
                result.push(RightParen);
                block(b, result);
            }
            Statement::While(c, b) => {
                result.extend([Keyword("while"), LeftParen]);
                c.tokenize(result, Precedence::Comma);
                result.push(RightParen);
                statement_block(b, result);
            }
            Statement::Continue => result.extend([Keyword("continue"), Semicolon]),
            Statement::Break => result.extend([Keyword("break"), Semicolon]),
        }
    }
}

fn block(code: Vec<Statement>, result: &mut Vec<Token>) {
    result.push(LeftBrace);
    for stmt in code {
        stmt.tokenize(result);
    }
    result.push(RightBrace);
}

fn statement_block(code: Vec<Statement>, result: &mut Vec<Token>) {
    match code.len() {
        0 => result.push(Semicolon),
        1 => code.into_iter().next().unwrap().tokenize(result),
        _ => block(code, result),
    }
}

fn number(n: Result<ParsedNumber, std::string::String>) -> Token {
    match n {
        Ok(pn) => Integer(pn),
        Err(n) => OtherNumber(n),
    }
}
