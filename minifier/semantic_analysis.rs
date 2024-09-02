use std::collections::HashMap;

use crate::parse::*;

pub struct Symbols {
    pub symbols: Vec<Identifier>,
}

struct Scope<'p> {
    parent: Option<&'p Scope<'p>>,
    variables: HashMap<String, usize>,
}

pub struct Identifier {
    kind: IdentKind,
    pub others_in_scope: Vec<usize>,
    pub occurances: usize,
}

enum IdentKind {
    Type(Struct),
    Ident(TypeOf),
}

#[derive(Clone)]
enum TypeOf {
    Unknown(Option<String>),
    User(usize),
    Pointer(Box<TypeOf>),
    Array(Box<TypeOf>, u64),
    Function(Box<TypeOf>),
}

#[derive(Default)]
struct Struct {
    members: HashMap<String, usize>,
}

impl<'a> Scope<'a> {
    fn new(parent: &'a Scope) -> Self {
        Scope {
            parent: Some(parent),
            variables: HashMap::new(),
        }
    }

    fn lookup(&self, name: &str) -> Option<usize> {
        self.variables
            .get(name)
            .copied()
            .or_else(|| self.parent?.lookup(name))
    }

    fn define(&mut self, name: String, id: usize) {
        self.variables.insert(name, id);
    }

    fn in_scope(&self, mut f: impl FnMut(usize)) {
        for &v in self.variables.values() {
            f(v);
        }
        if let Some(parent) = self.parent {
            parent.in_scope(f);
        }
    }
}

impl Symbols {
    fn declare(&mut self, base: &TypeOf, form: &DeclForm) -> usize {
        let id = self.symbols.len();
        self.symbols.push(Identifier {
            kind: IdentKind::Ident(self.get_type(base, form)),
            others_in_scope: vec![],
            occurances: 0,
        });
        id
    }

    fn declare_function(&mut self, ret_type: &TypeOf, function: &Function) -> usize {
        let id = self.symbols.len();
        self.symbols.push(Identifier {
            kind: IdentKind::Ident(TypeOf::Function(Box::new(
                self.get_type(ret_type, &function.decl_form),
            ))),
            others_in_scope: vec![],
            occurances: 0,
        });
        id
    }

    fn declare_type(&mut self) -> usize {
        let id = self.symbols.len();
        self.symbols.push(Identifier {
            kind: IdentKind::Type(Struct::default()),
            others_in_scope: vec![],
            occurances: 0,
        });
        id
    }

    fn get_type(&self, base: &TypeOf, form: &DeclForm) -> TypeOf {
        match form {
            DeclForm::OpEquals => TypeOf::Unknown(None),
            DeclForm::Name(_) => base.clone(),
            DeclForm::Pointer(f) | DeclForm::Array(f, None) => {
                TypeOf::Pointer(Box::new(self.get_type(base, f)))
            }
            DeclForm::LReference(f) => self.get_type(base, f),
            DeclForm::RReference(f) => self.get_type(base, f),
            DeclForm::Array(f, Some(n)) => {
                TypeOf::Array(Box::new(self.get_type(base, f)), n.as_ref().unwrap().value)
            }
            DeclForm::BitField(f, _) => self.get_type(base, f),
        }
    }
}

impl IdentKind {
    #[track_caller]
    fn unwrap_ident(&self) -> &TypeOf {
        match self {
            IdentKind::Type(_) => panic!("not an identifier"),
            IdentKind::Ident(ty) => ty,
        }
    }

    #[track_caller]
    fn unwrap_type(&self) -> &Struct {
        match self {
            IdentKind::Type(str) => str,
            IdentKind::Ident(_) => panic!("not a type"),
        }
    }

    #[track_caller]
    fn unwrap_type_mut(&mut self) -> &mut Struct {
        match self {
            IdentKind::Type(str) => str,
            IdentKind::Ident(_) => panic!("not a type"),
        }
    }
}

pub fn analyze(ast: &mut [TopLevel]) -> Symbols {
    let mut symbols = Symbols { symbols: vec![] };

    let mut scope = Scope {
        parent: None,
        variables: HashMap::new(),
    };

    for tl in ast {
        match tl {
            TopLevel::Declaration(d) => process_declaration(&mut symbols, &mut scope, d),
            TopLevel::Using(_) => {}
            TopLevel::Function(f) => process_function(&mut symbols, &mut scope, f),
            TopLevel::TypeDef(ty, form) => {
                process_base_type(&mut symbols, &mut scope, ty);
                if let Some(name) = get_name_mut(form) {
                    let id = symbols.declare_type();
                    scope.in_scope(|id2| {
                        symbols.symbols[id].others_in_scope.push(id2);
                        symbols.symbols[id2].others_in_scope.push(id);
                    });
                    scope.define(name.clone(), id);
                    *name = format!("${id}");
                }
            }
            TopLevel::Struct(name, members, vars) => {
                let id = symbols.declare_type();
                scope.in_scope(|id2| {
                    symbols.symbols[id].others_in_scope.push(id2);
                    symbols.symbols[id2].others_in_scope.push(id);
                });
                scope.define(name.clone(), id);
                *name = format!("${id}");
                let mut member_scope = Scope::new(&scope);
                for item in members {
                    match item {
                        MemberItem::Field(d) => {
                            process_declaration(&mut symbols, &mut member_scope, d);
                        }
                        MemberItem::Method(f) => {
                            process_function(&mut symbols, &mut member_scope, f);
                        }
                        MemberItem::Constructor(tyname, args, inits, body) => {
                            *tyname = name.clone();
                            symbols.symbols[id].occurances += 1;

                            let mut scope = Scope::new(&member_scope);
                            for (ty, arg) in args {
                                let ty = process_base_type(&mut symbols, &mut scope, ty);
                                process_declexpr(&mut symbols, &mut scope, &ty, arg);
                            }

                            for (member, init) in inits {
                                if let Some(id) = scope.lookup(member) {
                                    symbols.symbols[id].occurances += 1;
                                    *member = format!("${id}");
                                    process_init(&mut symbols, &mut scope, init);
                                }
                            }

                            process_statements(&mut symbols, &mut scope, body);
                        }
                        MemberItem::DefaultedConstructor(tyname) => {
                            *tyname = name.clone();
                            symbols.symbols[id].occurances += 1;
                        }
                    }
                    symbols.symbols[id].kind.unwrap_type_mut().members =
                        member_scope.variables.clone();
                }

                for declexpr in vars {
                    process_declexpr(&mut symbols, &mut scope, &TypeOf::User(id), declexpr);
                }
            }
        }
    }

    symbols
}

fn process_declaration(symbols: &mut Symbols, scope: &mut Scope, decl: &mut Declaration) {
    let ty = process_base_type(symbols, scope, &mut decl.base_type);
    for declexpr in &mut decl.declarations {
        process_declexpr(symbols, scope, &ty, declexpr);
    }
}

fn process_declexpr(
    symbols: &mut Symbols,
    scope: &mut Scope,
    base: &TypeOf,
    declexpr: &mut DeclExpr,
) {
    process_init(symbols, scope, &mut declexpr.init);
    let id = symbols.declare(&base, &declexpr.form);
    symbols.symbols[id].occurances += 1;
    scope.in_scope(|id2| {
        symbols.symbols[id].others_in_scope.push(id2);
        symbols.symbols[id2].others_in_scope.push(id);
    });
    if let Some(name) = get_name_mut(&mut declexpr.form) {
        scope.define(name.clone(), id);
        *name = format!("${id}");
    }
}

fn process_init(symbols: &mut Symbols, scope: &mut Scope, init: &mut Initializer) {
    match init {
        Initializer::Default => {}
        Initializer::Call(exprs) | Initializer::Brace(exprs) => {
            for e in exprs {
                process_expr(symbols, scope, e);
            }
        }
        Initializer::Equal(e) => {
            process_expr(symbols, scope, e);
        }
        Initializer::Array(exprs) => {
            for e in exprs {
                process_array_init(symbols, scope, e);
            }
        }
    }
}

fn process_array_init(symbols: &mut Symbols, scope: &mut Scope, init: &mut ArrayInit) {
    match init {
        ArrayInit::Expr(e) => {
            process_expr(symbols, scope, e);
        }
        ArrayInit::SubArray(exprs) => {
            for e in exprs {
                process_array_init(symbols, scope, e);
            }
        }
    }
}

fn process_function(symbols: &mut Symbols, scope: &mut Scope, f: &mut Function) {
    let ret = process_base_type(symbols, scope, &mut f.base_type);
    if get_name_mut(&mut f.decl_form).map(|s| &**s) != Some("main") {
        let id = symbols.declare_function(&ret, f);
        symbols.symbols[id].occurances += 1;
        scope.in_scope(|id2| {
            symbols.symbols[id].others_in_scope.push(id2);
            symbols.symbols[id2].others_in_scope.push(id);
        });
        if let Some(name) = get_name_mut(&mut f.decl_form) {
            scope.define(name.clone(), id);
            *name = format!("${id}");
        }
    }

    let mut scope = Scope::new(scope);

    for (ty, declexpr) in &mut f.args {
        let ty = process_base_type(symbols, &mut scope, ty);
        process_declexpr(symbols, &mut scope, &ty, declexpr);
    }

    process_statements(symbols, &mut scope, &mut f.body);
}

fn process_statements(symbols: &mut Symbols, scope: &mut Scope, block: &mut [Statement]) {
    let mut scope = Scope::new(scope);

    for stmt in block {
        match stmt {
            Statement::Declaration(d) => process_declaration(symbols, &mut scope, d),
            Statement::Expression(e) | Statement::Case(e) => {
                process_expr(symbols, &mut scope, e);
            }
            Statement::Try(b) => process_statements(symbols, &mut scope, b),
            Statement::For(decl, cond, inc, body) => {
                let mut scope = Scope::new(&scope);
                if let Some(decl) = decl {
                    process_declaration(symbols, &mut scope, decl);
                }
                if let Some(cond) = cond {
                    process_expr(symbols, &mut scope, cond);
                }
                if let Some(inc) = inc {
                    process_expr(symbols, &mut scope, inc);
                }
                process_statements(symbols, &mut scope, body);
            }
            Statement::ForEach((ty, decl), arr, body) => {
                let ty = process_base_type(symbols, &mut scope, ty);
                match arr {
                    Ok(e) => {
                        process_expr(symbols, &mut scope, e);
                    }
                    Err(es) => {
                        for e in es {
                            process_expr(symbols, &mut scope, e);
                        }
                    }
                }
                let mut scope = Scope::new(&scope);
                process_declexpr(symbols, &mut scope, &ty, decl);

                process_statements(symbols, &mut scope, body);
            }
            Statement::Return(e) => {
                if let Some(e) = e {
                    process_expr(symbols, &mut scope, e);
                }
            }
            Statement::If(cond, then, otherwise) => {
                process_expr(symbols, &mut scope, cond);
                process_statements(symbols, &mut scope, then);
                process_statements(symbols, &mut scope, otherwise);
            }
            Statement::Switch(cond, body) | Statement::While(cond, body) => {
                process_expr(symbols, &mut scope, cond);
                process_statements(symbols, &mut scope, body);
            }
            Statement::Continue => {}
            Statement::Break => {}
            Statement::Default => {}
        }
    }
}

fn process_expr(symbols: &mut Symbols, scope: &mut Scope, expr: &mut Expression) -> TypeOf {
    match &mut **expr {
        Expr::Number(_) => TypeOf::Unknown(None),
        Expr::String(_) => TypeOf::Unknown(None),
        Expr::Ident(name) => match scope.lookup(name) {
            Some(id) => {
                *name = format!("${id}");
                symbols.symbols[id].occurances += 1;
                symbols.symbols[id].kind.unwrap_ident().clone()
            }
            None => TypeOf::Unknown(None),
        },
        Expr::Lambda(captures, args, body) => {
            for name in captures {
                match scope.lookup(name) {
                    Some(id) => {
                        *name = format!("${id}");
                        symbols.symbols[id].occurances += 1;
                    }
                    None => todo!(),
                }
            }
            let mut scope = Scope::new(scope);
            for (ty, decl) in args {
                let ty = process_base_type(symbols, &mut scope, ty);
                process_declexpr(symbols, &mut scope, &ty, decl);
            }
            process_statements(symbols, &mut scope, body);
            TypeOf::Unknown(None)
        }
        Expr::Throw(e) => {
            process_expr(symbols, scope, e);
            TypeOf::Unknown(None)
        }
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
        | Expr::Mod(e1, e2) => {
            process_expr(symbols, scope, e1);
            process_expr(symbols, scope, e2)
        }
        Expr::Ternary(cond, e1, e2) => {
            process_expr(symbols, scope, cond);
            process_expr(symbols, scope, e1);
            process_expr(symbols, scope, e2)
        }
        Expr::Deref(e) => match process_expr(symbols, scope, e) {
            TypeOf::Pointer(t) => *t,
            TypeOf::Array(t, _) => *t,
            _ => TypeOf::Unknown(None),
        },
        Expr::AddressOf(e) => TypeOf::Pointer(Box::new(process_expr(symbols, scope, e))),
        Expr::UnaryPlus(e)
        | Expr::Negate(e)
        | Expr::Not(e)
        | Expr::Complement(e)
        | Expr::PreIncrement(e)
        | Expr::PreDecrement(e)
        | Expr::PostIncrement(e)
        | Expr::PostDecrement(e) => process_expr(symbols, scope, e),
        Expr::SizeOf(e) => {
            process_expr(symbols, scope, e);
            TypeOf::Unknown(Some("size_t".to_owned()))
        }
        Expr::SizeOfType(ty) => {
            process_modified_type(symbols, scope, ty);
            TypeOf::Unknown(Some("size_t".to_owned()))
        }
        Expr::Cast(ty, e) => {
            let ty = process_modified_type(symbols, scope, ty);
            process_expr(symbols, scope, e);
            ty
        }
        Expr::Index(arr, i) => {
            let ty = process_expr(symbols, scope, arr);
            process_expr(symbols, scope, i);
            match ty {
                TypeOf::Pointer(t) => *t,
                TypeOf::Array(t, _) => *t,
                _ => TypeOf::Unknown(None),
            }
        }
        Expr::Call(fun, args) => {
            let ty = process_expr(symbols, scope, fun);
            for arg in args {
                process_expr(symbols, scope, arg);
            }
            match ty {
                TypeOf::Function(ret) => *ret,
                _ => TypeOf::Unknown(None),
            }
        }
        Expr::Construct(ty, args) | Expr::BraceConstruct(ty, args) => {
            let ty = process_base_type(symbols, scope, ty);
            for arg in args {
                process_expr(symbols, scope, arg);
            }
            ty
        }
        Expr::MemberAccess(e, name) => {
            if let TypeOf::User(type_id) = process_expr(symbols, scope, e) {
                if let Some(&member_id) = symbols.symbols[type_id]
                    .kind
                    .unwrap_type()
                    .members
                    .get(name)
                {
                    *name = format!("${member_id}");
                    symbols.symbols[member_id].occurances += 1;
                    symbols.symbols[member_id].kind.unwrap_ident().clone()
                } else {
                    TypeOf::Unknown(None)
                }
            } else {
                TypeOf::Unknown(None)
            }
        }
        Expr::PointerMemberAccess(e, name) => {
            if let TypeOf::Pointer(ty) = process_expr(symbols, scope, e) {
                if let TypeOf::User(type_id) = *ty {
                    if let Some(&member_id) = symbols.symbols[type_id]
                        .kind
                        .unwrap_type()
                        .members
                        .get(name)
                    {
                        *name = format!("${member_id}");
                        symbols.symbols[member_id].occurances += 1;
                        symbols.symbols[member_id].kind.unwrap_ident().clone()
                    } else {
                        TypeOf::Unknown(None)
                    }
                } else {
                    TypeOf::Unknown(None)
                }
            } else {
                TypeOf::Unknown(None)
            }
        }
    }
}

fn process_modified_type(
    symbols: &mut Symbols,
    scope: &mut Scope,
    ty: &mut ModifiedType,
) -> TypeOf {
    match ty {
        ModifiedType::Base(base) => process_base_type(symbols, scope, base),
        ModifiedType::Pointer(ty) => {
            TypeOf::Pointer(Box::new(process_modified_type(symbols, scope, ty)))
        }
        ModifiedType::LReference(ty) | ModifiedType::RReference(ty) => {
            process_modified_type(symbols, scope, ty)
        }
    }
}

fn process_base_type(symbols: &mut Symbols, scope: &mut Scope, ty: &mut BaseType) -> TypeOf {
    let mut r = TypeOf::Unknown(None);
    if let CoreType::User(name) = &mut ty.core {
        if let Some(id) = scope.lookup(name) {
            symbols.symbols[id].occurances += 1;
            *name = format!("${id}");
            r = TypeOf::User(id);
        } else {
            r = TypeOf::Unknown(Some(name.clone()));
        }
    }
    for ty in ty.template_parameters.iter_mut().flatten() {
        process_base_type(symbols, scope, ty);
    }
    r
}

fn get_name_mut(form: &mut DeclForm) -> Option<&mut String> {
    match form {
        DeclForm::OpEquals => None,
        DeclForm::Name(v) => Some(v),
        DeclForm::Pointer(v)
        | DeclForm::LReference(v)
        | DeclForm::RReference(v)
        | DeclForm::Array(v, _)
        | DeclForm::BitField(v, _) => get_name_mut(v),
    }
}
