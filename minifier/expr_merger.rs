use crate::parse::*;

pub fn merge_exprs(ast: &mut [TopLevel]) {
    for tl in ast {
        match tl {
            TopLevel::Function(f) => merge_exprs_stmt(&mut f.body),
            TopLevel::Struct(_, i, _) => merge_exprs_item(i),
            _ => {}
        }
    }
}

fn merge_exprs_item(ast: &mut [MemberItem]) {
    for item in ast {
        match item {
            MemberItem::Method(f) => merge_exprs_stmt(&mut f.body),
            MemberItem::Constructor(_, _, _, b) => merge_exprs_stmt(b),
            _ => {}
        }
    }
}

fn merge_exprs_stmt(ast: &mut Vec<Statement>) {
    ast.dedup_by(|second, first| {
        match (first, second) {
            (Statement::Expression(e1), Statement::Expression(e2)) => {
                let mut d1 = dummy();
                let mut d2 = dummy();
                std::mem::swap(&mut d1, e1);
                std::mem::swap(&mut d2, e2);
                *e1 = Box::new(Expr::Comma(d1, d2));
                true
            }
            _ => false
        }
    });

    for stmt in ast {
        match stmt {
            Statement::Try(b) => merge_exprs_stmt(b),
            Statement::For(_, _, _, b) => merge_exprs_stmt(b),
            Statement::ForEach(_, _, b) => merge_exprs_stmt(b),
            Statement::If(_, t, f) => {
                merge_exprs_stmt(t);
                merge_exprs_stmt(f);
            }
            Statement::Switch(_, b) => merge_exprs_stmt(b),
            Statement::While(_, b) => merge_exprs_stmt(b),
            _ => {}
        }
    }
}

fn dummy() -> Expression {
    Box::new(Expr::Ident(String::new()))
}
