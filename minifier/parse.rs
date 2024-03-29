mod ast;
mod cpp;
mod lexical;
mod tokenize;

pub use ast::*;
pub use lexical::{ParsedNumber, PrefixString, Token, Lexer, string_concat};

pub fn parse(code: &[Token]) -> Vec<TopLevel> {
    let parser = cpp::FileParser::new();
    let parsed = parser.parse(
        code
            .iter()
            .enumerate()
            .map(|(i, t)| Ok((i, t.clone(), i + 1))),
    );

    match parsed {
        Err(lalrpop_util::ParseError::UnrecognizedToken { token, .. }) => {
            let mut packed = String::new();
            for (i, t) in code
                .iter()
                .enumerate()
                .skip(token.0.max(10) - 10)
                .take(20)
            {
                if i == token.0 {
                    packed.push_str("!!! ");
                }
                packed.push_str(&t.as_str());
                packed.push(' ');
            }
            eprintln!("{packed}");
            eprintln!("Failed to parse code, cannot continue.");
            std::process::exit(1);
        }
        v => v.unwrap(),
    }
}

pub fn unparse(ast: Vec<TopLevel>) -> Vec<Token> {
    tokenize::tokenize(ast)
}

pub fn stringify(tokens: Vec<Token>) -> String {
    let mut packed = String::new();
    for pair in tokens.windows(2) {
        packed.push_str(&pair[0].as_str());
        if pair[0].requires_space(&pair[1]) {
            packed.push(' ');
        }
    }
    packed.push_str(&tokens.last().unwrap().as_str());
    packed
}
