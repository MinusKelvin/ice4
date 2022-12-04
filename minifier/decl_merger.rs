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
            Statement::For(_, _, _, b) => merge_decls_stmt(b),
            Statement::ForEach(_, _, b) => merge_decls_stmt(b),
            Statement::If(_, t, f) => {
                merge_decls_stmt(t);
                merge_decls_stmt(f);
            }
            Statement::Switch(_, b) => merge_decls_stmt(b),
            Statement::While(_, b) => merge_decls_stmt(b),
            _ => {}
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

fn merge(d1: &mut Declaration, d2: &mut Declaration) -> bool {
    if d1.base_type == d2.base_type {
        d1.declarations.append(&mut d2.declarations);
        true
    } else {
        false
    }
}
