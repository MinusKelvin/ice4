use crate::parse::*;

pub fn fold_constants(ast: &mut [TopLevel]) {
    for tl in ast {
        match tl {
            TopLevel::Declaration(declaration) => fold_declaration(declaration),
            TopLevel::Function(function) => fold_function(function),
            TopLevel::Struct(_, member_items, decls) => {
                for member in member_items {
                    match member {
                        MemberItem::Field(declaration) => fold_declaration(declaration),
                        MemberItem::Method(function) => fold_function(function),
                        MemberItem::Constructor(_, args, inits, body) => {
                            for (_, decl) in args {
                                fold_init(&mut decl.init);
                            }
                            for (_, init) in inits {
                                fold_init(init);
                            }
                            fold_stmts(body);
                        }
                        MemberItem::DefaultedConstructor(_) => todo!(),
                    }
                }
                for decl in decls {
                    fold_init(&mut decl.init);
                }
            }
            _ => {}
        }
    }
}

fn fold_function(function: &mut Function) {
    for (_, decl) in &mut function.args {
        fold_init(&mut decl.init);
    }
    fold_stmts(&mut function.body);
}

fn fold_declaration(declaration: &mut Declaration) {
    for decl in &mut declaration.declarations {
        fold_init(&mut decl.init);
    }
}

fn fold_init(decl: &mut Initializer) {
    match decl {
        Initializer::Default => {}
        Initializer::Call(vec) | Initializer::Brace(vec) | Initializer::Array(vec) => {
            for expr in vec {
                fold_expr(expr);
            }
        }
        Initializer::Equal(expr) => fold_expr(expr),
    }
}

fn fold_stmts(stmts: &mut [Statement]) {
    for stmt in stmts {
        match stmt {
            Statement::Declaration(declaration) => fold_declaration(declaration),
            Statement::Expression(expr) => fold_expr(expr),
            Statement::Case(expr) => fold_expr(expr),
            Statement::Try(stmts) => fold_stmts(stmts),
            Statement::For(declaration, cond, incr, stmts) => {
                if let Some(declaration) = declaration {
                    fold_declaration(declaration);
                }
                if let Some(cond) = cond {
                    fold_expr(cond);
                }
                if let Some(incr) = incr {
                    fold_expr(incr);
                }
                fold_stmts(stmts);
            }
            Statement::ForEach((_, decl), range, stmts) => {
                fold_init(&mut decl.init);
                match range {
                    Ok(expr) => fold_expr(expr),
                    Err(array) => {
                        for expr in array {
                            fold_expr(expr);
                        }
                    }
                }
                fold_stmts(stmts);
            }
            Statement::Return(expr) => {
                if let Some(expr) = expr {
                    fold_expr(expr);
                }
            }
            Statement::If(expr, stmts_t, stmts_f) => {
                fold_expr(expr);
                fold_stmts(stmts_t);
                fold_stmts(stmts_f);
            }
            Statement::Switch(expr, stmts) => {
                fold_expr(expr);
                fold_stmts(stmts);
            }
            Statement::While(expr, stmts) => {
                fold_expr(expr);
                fold_stmts(stmts);
            }
            Statement::Continue => {}
            Statement::Break => {}
            Statement::Default => {}
        }
    }
}

fn fold_expr(expr: &mut Expr) {
    match expr {
        Expr::Number(_) => {}
        Expr::String(_) => {}
        Expr::Ident(_) => {}
        Expr::Lambda(_, args, stmts) => {
            for (_, decl) in args {
                fold_init(&mut decl.init);
            }
            fold_stmts(stmts);
        }
        Expr::MemberAccess(expr, _)
        | Expr::PointerMemberAccess(expr, _)
        | Expr::PostIncrement(expr)
        | Expr::PostDecrement(expr)
        | Expr::Deref(expr)
        | Expr::AddressOf(expr)
        | Expr::UnaryPlus(expr)
        | Expr::Negate(expr)
        | Expr::Not(expr)
        | Expr::NewArray(_, expr)
        | Expr::Complement(expr)
        | Expr::PreIncrement(expr)
        | Expr::PreDecrement(expr)
        | Expr::SizeOf(expr)
        | Expr::Cast(_, expr)
        | Expr::DeleteArray(expr)
        | Expr::Throw(expr) => fold_expr(expr),
        Expr::Comma(lhs, rhs)
        | Expr::Assign(lhs, rhs)
        | Expr::OrAssign(lhs, rhs)
        | Expr::XorAssign(lhs, rhs)
        | Expr::AndAssign(lhs, rhs)
        | Expr::LeftShiftAssign(lhs, rhs)
        | Expr::RightShiftAssign(lhs, rhs)
        | Expr::AddAssign(lhs, rhs)
        | Expr::SubAssign(lhs, rhs)
        | Expr::MulAssign(lhs, rhs)
        | Expr::DivAssign(lhs, rhs)
        | Expr::ModAssign(lhs, rhs)
        | Expr::LogicalOr(lhs, rhs)
        | Expr::LogicalAnd(lhs, rhs)
        | Expr::BitOr(lhs, rhs)
        | Expr::BitXor(lhs, rhs)
        | Expr::BitAnd(lhs, rhs)
        | Expr::Equals(lhs, rhs)
        | Expr::NotEquals(lhs, rhs)
        | Expr::Less(lhs, rhs)
        | Expr::LessEquals(lhs, rhs)
        | Expr::Greater(lhs, rhs)
        | Expr::GreaterEquals(lhs, rhs)
        | Expr::ShiftLeft(lhs, rhs)
        | Expr::ShiftRight(lhs, rhs)
        | Expr::Add(lhs, rhs)
        | Expr::Sub(lhs, rhs)
        | Expr::Mul(lhs, rhs)
        | Expr::Div(lhs, rhs)
        | Expr::Mod(lhs, rhs)
        | Expr::Index(lhs, rhs) => {
            fold_expr(lhs);
            fold_expr(rhs);
        }
        Expr::Ternary(cond, expr_t, expr_f) => {
            fold_expr(cond);
            fold_expr(expr_t);
            fold_expr(expr_f);
        }
        Expr::SizeOfType(_) => {}
        Expr::Call(expr, args) => {
            fold_expr(expr);
            for expr in args {
                fold_expr(expr);
            }
        }
        Expr::Construct(_, args) | Expr::BraceConstruct(_, args) => {
            for expr in args {
                fold_expr(expr);
            }
        }
    }

    match expr {
        Expr::LessEquals(lhs, rhs) => {
            if let Expr::Number(Ok(num)) = &**rhs {
                let lhs = take(lhs);
                let mut num = num.clone();
                num.value += 1;
                *expr = Expr::Less(lhs, Box::new(Expr::Number(Ok(num))));
            }
        }
        // this makes the compressed thing larger wtf?
        // Expr::BitOr(lhs, rhs) => {
        //     if let (Expr::Number(Ok(lhs)), Expr::Number(Ok(rhs))) = (&**lhs, &**rhs) {
        //         let mut num = lhs.clone();
        //         if lhs.suffix.is_empty() {
        //             num.suffix = rhs.suffix.clone();
        //         }
        //         num.value = lhs.value | rhs.value;
        //         *expr = Expr::Number(Ok(num));
        //     }
        // }
        Expr::Add(lhs, rhs) => {
            if let (Expr::Number(Ok(lhs)), Expr::Number(Ok(rhs))) = (&**lhs, &**rhs) {
                let mut num = lhs.clone();
                if lhs.suffix.is_empty() {
                    num.suffix = rhs.suffix.clone();
                }
                num.value = lhs.value + rhs.value;
                *expr = Expr::Number(Ok(num));
            } else if let Expr::Number(Ok(rhs)) = &**rhs {
                if rhs.value < 0 {
                    let mut num = rhs.clone();
                    num.value = -num.value;
                    *expr = Expr::Sub(take(lhs), Box::new(Expr::Number(Ok(num))));
                }
            }
        }
        Expr::Sub(lhs, rhs) => {
            if let (Expr::Number(Ok(lhs)), Expr::Number(Ok(rhs))) = (&**lhs, &**rhs) {
                let mut num = lhs.clone();
                if lhs.suffix.is_empty() {
                    num.suffix = rhs.suffix.clone();
                }
                num.value = lhs.value - rhs.value;
                *expr = Expr::Number(Ok(num));
            } else if let Expr::Number(Ok(rhs)) = &**rhs {
                if rhs.value < 0 {
                    let mut num = rhs.clone();
                    num.value = -num.value;
                    *expr = Expr::Add(take(lhs), Box::new(Expr::Number(Ok(num))));
                }
            }
        }
        Expr::Negate(operand) => {
            if let Expr::Number(Ok(num)) = &**operand {
                let mut num = num.clone();
                num.value = -num.value;
                *expr = Expr::Number(Ok(num));
            }
        }
        Expr::Mul(lhs, rhs) => {
            if let (Expr::Number(Ok(lhs)), Expr::Number(Ok(rhs))) = (&**lhs, &**rhs) {
                let mut num = lhs.clone();
                if lhs.suffix.is_empty() {
                    num.suffix = rhs.suffix.clone();
                }
                num.value = lhs.value * rhs.value;
                *expr = Expr::Number(Ok(num));
            }
        }
        _ => {}
    }
}

fn take(from: &mut Expression) -> Expression {
    let mut dummy = Box::new(Expr::Ident(String::new()));
    std::mem::swap(&mut dummy, from);
    dummy
}
