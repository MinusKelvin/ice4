use crate::lexical::*;

#[derive(Debug)]
pub enum TopLevel {
    Declaration(Declaration),
    Using(String),
}

#[derive(Debug)]
pub struct Declaration {
    pub base_type: BaseType,
    pub declarations: Vec<DeclExpr>,
}

#[derive(Debug)]
pub struct BaseType {
    pub core: CoreType,
    pub const_: bool,
    pub template_parameters: Option<Vec<BaseType>>,
}

#[derive(Debug)]
pub enum CoreType {
    User(String),
    SignedChar,
    SignedShort,
    SignedInt,
    SignedLong,
    UnsignedChar,
    UnsignedShort,
    UnsignedInt,
    UnsignedLong,
    Float,
    Double,
    Void,
    Auto,
}

#[derive(Debug)]
pub struct DeclExpr {
    pub form: DeclForm,
    pub init: Initializer,
}

#[derive(Debug)]
pub enum Initializer {
    Default,
    Call(Vec<Expression>),
    Brace(Vec<Expression>),
    Equal(Expression),
    Array(Vec<Expression>),
    Function(Vec<(BaseType, DeclExpr)>, Vec<()>),
}

#[derive(Debug)]
pub enum DeclForm {
    Name(String),
    Pointer(Box<DeclForm>),
    LReference(Box<DeclForm>),
    RReference(Box<DeclForm>),
    Array(Box<DeclForm>, Option<Result<ParsedNumber, String>>),
}

pub type Expression = Box<Expr>;

#[derive(Debug)]
pub enum Expr {
    Number(Result<ParsedNumber, String>),
    String(String),
    Ident(String),

    Comma(Expression, Expression),
    Ternary(Expression, Expression, Expression),
}
