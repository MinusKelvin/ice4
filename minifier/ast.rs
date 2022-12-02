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
}

#[derive(Debug)]
pub struct DeclExpr {
    pub form: DeclForm,
    pub init: Initializer,
}

#[derive(Debug)]
pub enum Initializer {
    Default,
    Call(()),
    Brace(()),
    Equal(()),
    Array(()),
}

#[derive(Debug)]
pub enum DeclForm {
    Name(String),
    Pointer(Box<DeclForm>),
    Array(Box<DeclForm>, Option<Result<ParsedNumber, String>>),
}
