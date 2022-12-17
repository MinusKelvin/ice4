use super::lexical::*;

#[derive(Debug)]
pub enum TopLevel {
    Declaration(Declaration),
    Using(String),
    Function(Function),
    Struct(String, Vec<MemberItem>, Vec<DeclExpr>),
}

#[derive(Debug)]
pub struct Function {
    pub base_type: BaseType,
    pub decl_form: DeclForm,
    pub args: Vec<(BaseType, DeclExpr)>,
    pub body: Vec<Statement>,
}

#[derive(Debug)]
pub struct Declaration {
    pub base_type: BaseType,
    pub declarations: Vec<DeclExpr>,
}

#[derive(Debug)]
pub enum MemberItem {
    Field(Declaration),
    Method(Function),
    Constructor(String, Vec<(BaseType, DeclExpr)>, Vec<(String, Initializer)>, Vec<Statement>),
    DefaultedConstructor(String),
}

#[derive(Debug, PartialEq, Eq)]
pub struct BaseType {
    pub core: CoreType,
    pub const_: bool,
    pub template_parameters: Option<Vec<BaseType>>,
}

#[derive(Debug, PartialEq, Eq)]
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
    Array(ArrayInit),
}

#[derive(Debug)]
pub enum ArrayInit {
    Exprs(Vec<Expression>),
    Arrays(Vec<ArrayInit>),
}

#[derive(Debug)]
pub enum DeclForm {
    OpEquals,
    Name(String),
    Pointer(Box<DeclForm>),
    LReference(Box<DeclForm>),
    RReference(Box<DeclForm>),
    Array(Box<DeclForm>, Option<Expression>),
}

#[derive(Debug)]
pub enum ModifiedType {
    Base(BaseType),
    Pointer(Box<ModifiedType>),
    LReference(Box<ModifiedType>),
    RReference(Box<ModifiedType>),
}

pub type Expression = Box<Expr>;

#[derive(Debug)]
pub enum Expr {
    Number(Result<ParsedNumber, String>),
    String(String),
    Ident(String),
    Lambda(Vec<String>, Vec<(BaseType, DeclExpr)>, Vec<Statement>),

    Comma(Expression, Expression),
    Throw(Expression),
    Assign(Expression, Expression),
    OrAssign(Expression, Expression),
    XorAssign(Expression, Expression),
    AndAssign(Expression, Expression),
    LeftShiftAssign(Expression, Expression),
    RightShiftAssign(Expression, Expression),
    AddAssign(Expression, Expression),
    SubAssign(Expression, Expression),
    MulAssign(Expression, Expression),
    DivAssign(Expression, Expression),
    ModAssign(Expression, Expression),
    Ternary(Expression, Expression, Expression),
    LogicalOr(Expression, Expression),
    LogicalAnd(Expression, Expression),
    BitOr(Expression, Expression),
    BitXor(Expression, Expression),
    BitAnd(Expression, Expression),
    Equals(Expression, Expression),
    NotEquals(Expression, Expression),
    Less(Expression, Expression),
    LessEquals(Expression, Expression),
    Greater(Expression, Expression),
    GreaterEquals(Expression, Expression),
    ShiftLeft(Expression, Expression),
    ShiftRight(Expression, Expression),
    Add(Expression, Expression),
    Sub(Expression, Expression),
    Mul(Expression, Expression),
    Div(Expression, Expression),
    Mod(Expression, Expression),
    Deref(Expression),
    AddressOf(Expression),
    UnaryPlus(Expression),
    Negate(Expression),
    Not(Expression),
    Complement(Expression),
    PreIncrement(Expression),
    PreDecrement(Expression),
    SizeOf(Expression),
    SizeOfType(ModifiedType),
    Cast(ModifiedType, Expression),
    Index(Expression, Expression),
    Call(Expression, Vec<Expression>),
    Construct(BaseType, Vec<Expression>),
    BraceConstruct(BaseType, Vec<Expression>),
    MemberAccess(Expression, String),
    PointerMemberAccess(Expression, String),
    PostIncrement(Expression),
    PostDecrement(Expression),
}

#[derive(Debug)]
pub enum Statement {
    Declaration(Declaration),
    Expression(Expression),
    Case(Expression),
    Try(Vec<Statement>),
    For(Option<Declaration>, Option<Expression>, Option<Expression>, Vec<Statement>),
    ForEach((BaseType, DeclExpr), Result<Expression, Vec<Expression>>, Vec<Statement>),
    Return(Option<Expression>),
    If(Expression, Vec<Statement>, Vec<Statement>),
    Switch(Expression, Vec<Statement>),
    While(Expression, Vec<Statement>),
    Continue,
    Break,
}
