use crate::lexical::*;

#[derive(Debug)]
pub enum TopLevel {
    Declaration(Declaration),
}

#[derive(Debug)]
pub struct Declaration {
    pub base_type: BaseType,
    pub declarations: Vec<DeclExpr>,
}

#[derive(Debug)]
pub struct BaseType {
    pub path: Vec<String>,
    pub template_parameters: Option<Vec<BaseType>>,
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
