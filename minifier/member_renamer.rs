use std::collections::{HashMap, HashSet};

use crate::parse::*;

#[derive(Default)]
struct StructInfo {
    names: HashSet<String>,
    numbers: HashSet<usize>,
}

pub fn rename_members(ast: &mut [TopLevel]) {
    let mut counts = HashMap::new();

    let mut structs = HashMap::new();

    for tl in &mut *ast {
        match tl {
            TopLevel::Declaration(d) => {
                for decl in &d.declarations {
                    count_occurances_init(&mut counts, &decl.init);
                }
            }
            TopLevel::Using(_) => {}
            TopLevel::Function(f) => {
                count_occurances_stmt(&mut counts, &f.body);
            }
            TopLevel::Struct(n, items, _) => {
                let members: &mut StructInfo = structs.entry(n).or_default();
                for item in items {
                    match item {
                        MemberItem::Field(d) => {
                            for decl in &d.declarations {
                                count_occurances_decl(&mut counts, decl);
                                let mut form = &decl.form;
                                loop {
                                    match form {
                                        DeclForm::OpEquals => break,
                                        DeclForm::Name(name) => {
                                            members.names.insert(name.clone());
                                            break;
                                        }
                                        DeclForm::Pointer(f) => form = f,
                                        DeclForm::LReference(f) => form = f,
                                        DeclForm::RReference(f) => form = f,
                                        DeclForm::Array(f, _) => form = f,
                                    }
                                }
                            }
                        }
                        MemberItem::Method(f) => {
                            let mut form = &f.decl_form;
                            loop {
                                match form {
                                    DeclForm::OpEquals => break,
                                    DeclForm::Name(name) => {
                                        members.names.insert(name.clone());
                                        break;
                                    }
                                    DeclForm::Pointer(f) => form = f,
                                    DeclForm::LReference(f) => form = f,
                                    DeclForm::RReference(f) => form = f,
                                    DeclForm::Array(f, _) => form = f,
                                }
                            }
                            count_occurances_declform(&mut counts, &f.decl_form);
                            count_occurances_stmt(&mut counts, &f.body);
                        }
                        MemberItem::Constructor(_, _, inits, body) => {
                            for (name, init) in inits {
                                if let Some(c) = counts.get_mut(&**name) {
                                    *c += 1;
                                }
                                count_occurances_init(&mut counts, init);
                            }
                            count_occurances_stmt(&mut counts, body);
                        }
                        MemberItem::DefaultedConstructor(_) => {}
                    }
                }
            }
        }
    }

    let num_names = structs.values().map(|n| n.names.len()).max().unwrap();
    for s in structs.values_mut() {
        s.numbers.extend(0..num_names);
    }

    let mut renaming_order: Vec<_> = counts.into_iter().collect();
    renaming_order.sort_by_key(|&(n, count)| (std::cmp::Reverse(count), n));

    let mut translation_table = HashMap::new();
    for (name, _) in renaming_order {
        let mut valid_names: HashSet<usize> = (0..num_names).collect();
        for struc in structs.values() {
            if struc.names.contains(name) {
                valid_names.retain(|n| struc.numbers.contains(n));
            }
        }
        let number = valid_names.into_iter().min().unwrap();
        for struc in structs.values_mut() {
            if struc.names.contains(name) {
                struc.numbers.remove(&number);
            }
        }

        translation_table.insert(name.to_owned(), number.to_string());
    }

    for tl in ast {
        match tl {
            TopLevel::Declaration(d) => {
                for decl in &mut d.declarations {
                    rename_init(&translation_table, &mut decl.init);
                }
            },
            TopLevel::Using(_) => {},
            TopLevel::Function(f) => {
                rename_stmt(&translation_table, &mut f.body);
            },
            TopLevel::Struct(_, members, _) => {
                for item in members {
                    match item {
                        MemberItem::Field(d) => {
                            for decl in &mut d.declarations {
                                rename_decl(&translation_table, decl);
                            }
                        }
                        MemberItem::Method(f) => {
                            rename_declform(&translation_table, &mut f.decl_form);
                            rename_stmt(&translation_table, &mut f.body);
                        },
                        MemberItem::Constructor(_, _, inits, body) => {
                            for (n, init) in inits {
                                *n = translation_table.get(n).unwrap().clone();
                                rename_init(&translation_table, init);
                            }
                            rename_stmt(&translation_table, body);
                        },
                        MemberItem::DefaultedConstructor(_) => {},
                    }
                }
            },
        }
    }
}

fn count_occurances_decl<'a>(counts: &mut HashMap<&'a str, usize>, decl: &'a DeclExpr) {
    count_occurances_init(counts, &decl.init);
    count_occurances_declform(counts, &decl.form);
}

fn count_occurances_init<'a>(counts: &mut HashMap<&'a str, usize>, init: &'a Initializer) {
    match init {
        Initializer::Default => {}
        Initializer::Call(exprs) | Initializer::Brace(exprs) | Initializer::Array(exprs) => {
            for e in exprs {
                count_occurances_expr(counts, e);
            }
        }
        Initializer::Equal(e) => count_occurances_expr(counts, e),
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
        | Expr::Cast(_, e) => {
            count_occurances_expr(counts, e);
        }
        Expr::MemberAccess(e, m) | Expr::PointerMemberAccess(e, m) => {
            count_occurances_expr(counts, e);
            if let Some(c) = counts.get_mut(&**m) {
                *c += 1;
            }
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
            Statement::Declaration(_) => {}
            Statement::Expression(e) => count_occurances_expr(counts, e),
            Statement::Case(_) => {}
            Statement::Try(b) => count_occurances_stmt(counts, b),
            Statement::For(_, c, i, b) => {
                if let Some(c) = c {
                    count_occurances_expr(counts, c);
                }
                if let Some(i) = i {
                    count_occurances_expr(counts, i);
                }
                count_occurances_stmt(counts, b);
            }
            Statement::ForEach(_, expr, b) => {
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
            Statement::Default => {}
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
        Initializer::Call(exprs) | Initializer::Brace(exprs) | Initializer::Array(exprs) => {
            for e in exprs {
                rename_expr(translation, e);
            }
        }
        Initializer::Equal(e) => rename_expr(translation, e),
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
        | Expr::Cast(_, e) => {
            rename_expr(translation, e);
        }
        Expr::MemberAccess(e, n)
        | Expr::PointerMemberAccess(e, n) => {
            if let Some(t) = translation.get(n) {
                *n = t.clone();
            }
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
            Statement::Default => {}
        }
    }
}
