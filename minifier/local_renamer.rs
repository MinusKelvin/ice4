use std::collections::HashMap;

use crate::parse::*;

pub fn rename_locals(ast: &mut [TopLevel]) {
    for tl in ast {
        match tl {
            TopLevel::Function(f) => rename_function_locals(&mut f.args, &mut [], &mut f.body),
            TopLevel::Struct(_, i, _) => {
                for item in i {
                    match item {
                        MemberItem::Method(f) => {
                            rename_function_locals(&mut f.args, &mut [], &mut f.body)
                        }
                        MemberItem::Constructor(_, args, init, body) => {
                            rename_function_locals(args, init, body)
                        }
                        _ => {}
                    }
                }
            }
            _ => {}
        }
    }
}

fn rename_function_locals(
    args: &mut [(BaseType, DeclExpr)],
    init: &mut [(String, Initializer)],
    body: &mut [Statement],
) {
    let mut counts = HashMap::new();
    for (_, decl) in &*args {
        count_occurances_decl(&mut counts, decl);
    }
    for (_, init) in &*init {
        count_occurances_init(&mut counts, init);
    }
    count_occurances_stmt(&mut counts, body);

    let mut translation_table = crate::renamer::make_translation_table(counts);
    for v in translation_table.values_mut() {
        v.push_str("_L");
    }

    for (_, decl) in args {
        rename_decl(&translation_table, decl);
    }
    for (_, init) in init {
        rename_init(&translation_table, init);
    }
    rename_stmt(&translation_table, body);
}

fn count_occurances_decl<'a>(counts: &mut HashMap<&'a str, usize>, decl: &'a DeclExpr) {
    count_occurances_init(counts, &decl.init);
    count_occurances_declform(counts, &decl.form);
}

fn count_occurances_init<'a>(counts: &mut HashMap<&'a str, usize>, init: &'a Initializer) {
    match init {
        Initializer::Default => {}
        Initializer::Call(exprs) | Initializer::Brace(exprs) => {
            for e in exprs {
                count_occurances_expr(counts, e);
            }
        }
        Initializer::Array(arr) => count_occurances_array(counts, arr),
        Initializer::Equal(e) => count_occurances_expr(counts, e),
    }
}

fn count_occurances_array<'a>(counts: &mut HashMap<&'a str, usize>, arr: &'a ArrayInit) {
    match arr {
        ArrayInit::Exprs(exprs) => {
            for e in exprs {
                count_occurances_expr(counts, e);
            }
        }
        ArrayInit::Arrays(arrs) => {
            for a in arrs {
                count_occurances_array(counts, a);
            }
        }
    }
}

fn count_occurances_declform<'a>(counts: &mut HashMap<&'a str, usize>, form: &'a DeclForm) {
    match form {
        DeclForm::OpEquals => {}
        DeclForm::Name(n) => {
            counts.insert(n, 1);
        }
        DeclForm::Pointer(f)
        | DeclForm::LReference(f)
        | DeclForm::RReference(f)
        | DeclForm::Array(f, _) => count_occurances_declform(counts, f),
    }
}

fn count_occurances_expr<'a>(counts: &mut HashMap<&'a str, usize>, expr: &'a Expression) {
    match &**expr {
        Expr::Number(_) => {}
        Expr::String(_) => {}
        Expr::Ident(n) => {
            if let Some(c) = counts.get_mut(&**n) {
                *c += 1;
            }
        }
        Expr::Lambda(caps, args, body) => {
            for n in caps {
                if let Some(c) = counts.get_mut(&**n) {
                    *c += 1;
                }
            }
            for (_, decl) in args {
                count_occurances_decl(counts, decl);
            }
            count_occurances_stmt(counts, body);
        }
        Expr::Comma(l, r)
        | Expr::Assign(l, r)
        | Expr::OrAssign(l, r)
        | Expr::XorAssign(l, r)
        | Expr::AndAssign(l, r)
        | Expr::LeftShiftAssign(l, r)
        | Expr::RightShiftAssign(l, r)
        | Expr::AddAssign(l, r)
        | Expr::SubAssign(l, r)
        | Expr::MulAssign(l, r)
        | Expr::DivAssign(l, r)
        | Expr::ModAssign(l, r)
        | Expr::LogicalOr(l, r)
        | Expr::LogicalAnd(l, r)
        | Expr::BitOr(l, r)
        | Expr::BitXor(l, r)
        | Expr::BitAnd(l, r)
        | Expr::Equals(l, r)
        | Expr::NotEquals(l, r)
        | Expr::Less(l, r)
        | Expr::LessEquals(l, r)
        | Expr::Greater(l, r)
        | Expr::GreaterEquals(l, r)
        | Expr::ShiftLeft(l, r)
        | Expr::ShiftRight(l, r)
        | Expr::Add(l, r)
        | Expr::Sub(l, r)
        | Expr::Mul(l, r)
        | Expr::Div(l, r)
        | Expr::Mod(l, r)
        | Expr::Index(l, r) => {
            count_occurances_expr(counts, l);
            count_occurances_expr(counts, r);
        }
        Expr::Ternary(c, t, f) => {
            count_occurances_expr(counts, c);
            count_occurances_expr(counts, t);
            count_occurances_expr(counts, f);
        }
        Expr::Deref(e)
        | Expr::Throw(e)
        | Expr::AddressOf(e)
        | Expr::UnaryPlus(e)
        | Expr::Negate(e)
        | Expr::Not(e)
        | Expr::Complement(e)
        | Expr::PreIncrement(e)
        | Expr::PreDecrement(e)
        | Expr::SizeOf(e)
        | Expr::PostIncrement(e)
        | Expr::PostDecrement(e)
        | Expr::Cast(_, e)
        | Expr::MemberAccess(e, _)
        | Expr::PointerMemberAccess(e, _) => {
            count_occurances_expr(counts, e);
        }
        Expr::SizeOfType(_) => {}
        Expr::Call(c, a) => {
            count_occurances_expr(counts, c);
            for e in a {
                count_occurances_expr(counts, e);
            }
        }
        Expr::Construct(_, a) | Expr::BraceConstruct(_, a) => {
            for e in a {
                count_occurances_expr(counts, e);
            }
        }
    }
}

fn count_occurances_stmt<'a>(counts: &mut HashMap<&'a str, usize>, stmts: &'a [Statement]) {
    for stmt in stmts {
        match stmt {
            Statement::Declaration(d) => {
                for decl in &d.declarations {
                    count_occurances_decl(counts, decl);
                }
            }
            Statement::Expression(e) => count_occurances_expr(counts, e),
            Statement::Case(_) => {}
            Statement::Try(b) => count_occurances_stmt(counts, b),
            Statement::For(d, c, i, b) => {
                if let Some(d) = d {
                    for decl in &d.declarations {
                        count_occurances_decl(counts, decl);
                    }
                }
                if let Some(c) = c {
                    count_occurances_expr(counts, c);
                }
                if let Some(i) = i {
                    count_occurances_expr(counts, i);
                }
                count_occurances_stmt(counts, b);
            }
            Statement::ForEach((_, d), expr, b) => {
                count_occurances_decl(counts, d);
                match expr {
                    Ok(e) => count_occurances_expr(counts, e),
                    Err(es) => {
                        for e in es {
                            count_occurances_expr(counts, e);
                        }
                    }
                }
                count_occurances_stmt(counts, b);
            }
            Statement::Return(e) => {
                if let Some(e) = e {
                    count_occurances_expr(counts, e);
                }
            }
            Statement::If(c, t, f) => {
                count_occurances_expr(counts, c);
                count_occurances_stmt(counts, t);
                count_occurances_stmt(counts, f);
            }
            Statement::Switch(c, b) | Statement::While(c, b) => {
                count_occurances_expr(counts, c);
                count_occurances_stmt(counts, b);
            }
            Statement::Continue => {}
            Statement::Break => {}
        }
    }
}

fn rename_decl(translation: &HashMap<String, String>, decl: &mut DeclExpr) {
    rename_declform(translation, &mut decl.form);
    rename_init(translation, &mut decl.init);
}

fn rename_init(translation: &HashMap<String, String>, init: &mut Initializer) {
    match init {
        Initializer::Default => {}
        Initializer::Call(exprs) | Initializer::Brace(exprs) => {
            for e in exprs {
                rename_expr(translation, e);
            }
        }
        Initializer::Array(a) => rename_array(translation, a),
        Initializer::Equal(e) => rename_expr(translation, e),
    }
}

fn rename_array(translation: &HashMap<String, String>, arr: &mut ArrayInit) {
    match arr {
        ArrayInit::Exprs(exprs) => for e in exprs {
            rename_expr(translation, e);
        },
        ArrayInit::Arrays(arrs) => for a in arrs {
            rename_array(translation, a);
        },
    }
}

fn rename_declform(translation: &HashMap<String, String>, form: &mut DeclForm) {
    match form {
        DeclForm::OpEquals => {}
        DeclForm::Name(n) => {
            if let Some(t) = translation.get(n) {
                *n = t.clone();
            }
        }
        DeclForm::Pointer(f)
        | DeclForm::LReference(f)
        | DeclForm::RReference(f)
        | DeclForm::Array(f, _) => rename_declform(translation, f),
    }
}

fn rename_expr(translation: &HashMap<String, String>, expr: &mut Expression) {
    match &mut **expr {
        Expr::Number(_) => {}
        Expr::String(_) => {}
        Expr::Ident(n) => {
            if let Some(t) = translation.get(n) {
                *n = t.clone();
            }
        }
        Expr::Lambda(caps, args, body) => {
            for n in caps {
                if let Some(t) = translation.get(n) {
                    *n = t.clone();
                }
            }
            for (_, decl) in args {
                rename_decl(translation, decl);
            }
            rename_stmt(translation, body);
        }
        Expr::Comma(l, r)
        | Expr::Assign(l, r)
        | Expr::OrAssign(l, r)
        | Expr::XorAssign(l, r)
        | Expr::AndAssign(l, r)
        | Expr::LeftShiftAssign(l, r)
        | Expr::RightShiftAssign(l, r)
        | Expr::AddAssign(l, r)
        | Expr::SubAssign(l, r)
        | Expr::MulAssign(l, r)
        | Expr::DivAssign(l, r)
        | Expr::ModAssign(l, r)
        | Expr::LogicalOr(l, r)
        | Expr::LogicalAnd(l, r)
        | Expr::BitOr(l, r)
        | Expr::BitXor(l, r)
        | Expr::BitAnd(l, r)
        | Expr::Equals(l, r)
        | Expr::NotEquals(l, r)
        | Expr::Less(l, r)
        | Expr::LessEquals(l, r)
        | Expr::Greater(l, r)
        | Expr::GreaterEquals(l, r)
        | Expr::ShiftLeft(l, r)
        | Expr::ShiftRight(l, r)
        | Expr::Add(l, r)
        | Expr::Sub(l, r)
        | Expr::Mul(l, r)
        | Expr::Div(l, r)
        | Expr::Mod(l, r)
        | Expr::Index(l, r) => {
            rename_expr(translation, l);
            rename_expr(translation, r);
        }
        Expr::Ternary(c, t, f) => {
            rename_expr(translation, c);
            rename_expr(translation, t);
            rename_expr(translation, f);
        }
        Expr::Deref(e)
        | Expr::Throw(e)
        | Expr::AddressOf(e)
        | Expr::UnaryPlus(e)
        | Expr::Negate(e)
        | Expr::Not(e)
        | Expr::Complement(e)
        | Expr::PreIncrement(e)
        | Expr::PreDecrement(e)
        | Expr::SizeOf(e)
        | Expr::PostIncrement(e)
        | Expr::PostDecrement(e)
        | Expr::Cast(_, e)
        | Expr::MemberAccess(e, _)
        | Expr::PointerMemberAccess(e, _) => {
            rename_expr(translation, e);
        }
        Expr::SizeOfType(_) => {}
        Expr::Call(c, a) => {
            rename_expr(translation, c);
            for e in a {
                rename_expr(translation, e);
            }
        }
        Expr::Construct(_, a) | Expr::BraceConstruct(_, a) => {
            for e in a {
                rename_expr(translation, e);
            }
        }
    }
}

fn rename_stmt(translation: &HashMap<String, String>, stmts: &mut [Statement]) {
    for stmt in stmts {
        match stmt {
            Statement::Declaration(d) => {
                for decl in &mut d.declarations {
                    rename_decl(translation, decl);
                }
            }
            Statement::Expression(e) => rename_expr(translation, e),
            Statement::Case(_) => {}
            Statement::Try(b) => rename_stmt(translation, b),
            Statement::For(d, c, i, b) => {
                if let Some(d) = d {
                    for decl in &mut d.declarations {
                        rename_decl(translation, decl);
                    }
                }
                if let Some(c) = c {
                    rename_expr(translation, c);
                }
                if let Some(i) = i {
                    rename_expr(translation, i);
                }
                rename_stmt(translation, b);
            }
            Statement::ForEach((_, d), expr, b) => {
                rename_decl(translation, d);
                match expr {
                    Ok(e) => rename_expr(translation, e),
                    Err(es) => {
                        for e in es {
                            rename_expr(translation, e);
                        }
                    }
                }
                rename_stmt(translation, b);
            }
            Statement::Return(e) => {
                if let Some(e) = e {
                    rename_expr(translation, e);
                }
            }
            Statement::If(c, t, f) => {
                rename_expr(translation, c);
                rename_stmt(translation, t);
                rename_stmt(translation, f);
            }
            Statement::Switch(c, b) | Statement::While(c, b) => {
                rename_expr(translation, c);
                rename_stmt(translation, b);
            }
            Statement::Continue => {}
            Statement::Break => {}
        }
    }
}
