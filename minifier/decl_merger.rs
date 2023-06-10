use crate::parse::*;

pub fn merge_decls(ast: &mut Vec<TopLevel>) {
    ast.dedup_by(|second, first| {
        if let (TopLevel::Declaration(d1), TopLevel::Declaration(d2)) = (first, second) {
            merge(d1, d2)
        } else {
            false
        }
    });

    for tl in ast {
        match tl {
            TopLevel::Function(f) => merge_decls_stmt(&mut f.body),
            TopLevel::Struct(_, i, _) => merge_decls_item(i),
            _ => {}
        }
    }
}

fn merge_decls_stmt(ast: &mut Vec<Statement>) {
    ast.dedup_by(|second, first| {
        if let (Statement::Declaration(d1), Statement::Declaration(d2)) = (first, second) {
            merge(d1, d2)
        } else {
            false
        }
    });

    for stmt in ast {
        match stmt {
            Statement::Try(b) => merge_decls_stmt(b),
            Statement::For(_, c, i, b) => {
                if let Some(c) = c {
                    merge_decls_expr(c);
                }
                if let Some(i) = i {
                    merge_decls_expr(i);
                }
                merge_decls_stmt(b);
            }
            Statement::ForEach(_, r, b) => {
                match r {
                    Ok(e) => merge_decls_expr(e),
                    Err(v) => {
                        for e in v {
                            merge_decls_expr(e)
                        }
                    }
                }
                merge_decls_stmt(b);
            }
            Statement::If(e, t, f) => {
                merge_decls_expr(e);
                merge_decls_stmt(t);
                merge_decls_stmt(f);
            }
            Statement::Switch(e, b) | Statement::While(e, b) => {
                merge_decls_expr(e);
                merge_decls_stmt(b);
            }
            Statement::Expression(e) | Statement::Return(Some(e)) => merge_decls_expr(e),
            Statement::Declaration(_)
            | Statement::Return(None)
            | Statement::Case(_)
            | Statement::Label(_)
            | Statement::Goto(_)
            | Statement::Continue
            | Statement::Break
            | Statement::Default => {}
        }
    }
}

fn merge_decls_item(ast: &mut Vec<MemberItem>) {
    ast.dedup_by(|second, first| {
        if let (MemberItem::Field(d1), MemberItem::Field(d2)) = (first, second) {
            merge(d1, d2)
        } else {
            false
        }
    });

    for item in ast {
        match item {
            MemberItem::Method(f) => merge_decls_stmt(&mut f.body),
            MemberItem::Constructor(_, _, _, b) => merge_decls_stmt(b),
            _ => {}
        }
    }
}

fn merge_decls_expr(ast: &mut Expr) {
    match ast {
        Expr::Number(_) | Expr::String(_) | Expr::Ident(_) | Expr::SizeOfType(_) => {}
        Expr::Lambda(_, body) => merge_decls_stmt(body),
        Expr::Comma(e1, e2)
        | Expr::Assign(e1, e2)
        | Expr::OrAssign(e1, e2)
        | Expr::XorAssign(e1, e2)
        | Expr::AndAssign(e1, e2)
        | Expr::LeftShiftAssign(e1, e2)
        | Expr::RightShiftAssign(e1, e2)
        | Expr::AddAssign(e1, e2)
        | Expr::SubAssign(e1, e2)
        | Expr::MulAssign(e1, e2)
        | Expr::DivAssign(e1, e2)
        | Expr::ModAssign(e1, e2)
        | Expr::Ternary(e1, e2, _)
        | Expr::LogicalOr(e1, e2)
        | Expr::LogicalAnd(e1, e2)
        | Expr::BitOr(e1, e2)
        | Expr::BitXor(e1, e2)
        | Expr::BitAnd(e1, e2)
        | Expr::Equals(e1, e2)
        | Expr::NotEquals(e1, e2)
        | Expr::Less(e1, e2)
        | Expr::LessEquals(e1, e2)
        | Expr::Greater(e1, e2)
        | Expr::GreaterEquals(e1, e2)
        | Expr::ShiftLeft(e1, e2)
        | Expr::ShiftRight(e1, e2)
        | Expr::Add(e1, e2)
        | Expr::Sub(e1, e2)
        | Expr::Mul(e1, e2)
        | Expr::Div(e1, e2)
        | Expr::Mod(e1, e2)
        | Expr::Index(e1, e2) => {
            merge_decls_expr(e1);
            merge_decls_expr(e2);
        }
        Expr::Throw(e)
        | Expr::Deref(e)
        | Expr::AddressOf(e)
        | Expr::UnaryPlus(e)
        | Expr::Negate(e)
        | Expr::Not(e)
        | Expr::Complement(e)
        | Expr::PreIncrement(e)
        | Expr::PreDecrement(e)
        | Expr::SizeOf(e)
        | Expr::MemberAccess(e, _)
        | Expr::PointerMemberAccess(e, _)
        | Expr::PostIncrement(e)
        | Expr::PostDecrement(e)
        | Expr::Cast(_, e) => merge_decls_expr(e),
        Expr::Call(e, v) => {
            merge_decls_expr(e);
            for e in v {
                merge_decls_expr(e);
            }
        }
        Expr::Construct(_, v) | Expr::BraceConstruct(_, v) => {
            for e in v {
                merge_decls_expr(e)
            }
        }
    }
}

fn merge(d1: &mut Declaration, d2: &mut Declaration) -> bool {
    if d1.base_type == d2.base_type {
        d1.declarations.append(&mut d2.declarations);
        true
    } else {
        false
    }
}
