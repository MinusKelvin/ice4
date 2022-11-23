use std::iter::Peekable;

use crate::lexical::Token;

pub enum TokenTree {
    Parenthesis(Vec<TokenTree>),
    Brackets(Vec<TokenTree>),
    Braces(Vec<TokenTree>),
    Token(Token),
}

pub fn treeify(tokens: impl IntoIterator<Item = Token>) -> Vec<TokenTree> {
    let mut stream = tokens.into_iter().peekable();
    let result = from_tokens(&mut stream);
    assert!(stream.next().is_none(), "Unbalanced bracket pairs");
    result
}

fn from_tokens<I: Iterator<Item = Token>>(stream: &mut Peekable<I>) -> Vec<TokenTree> {
    let mut result = vec![];
    loop {
        match stream.peek() {
            None | Some(Token::RightParen | Token::RightBracket | Token::RightBrace) => {
                return result
            }
            Some(Token::LeftParen) => {
                stream.next();
                result.push(TokenTree::Parenthesis(from_tokens(stream)));
                assert_eq!(stream.next(), Some(Token::RightParen));
            }
            Some(Token::LeftBracket) => {
                stream.next();
                result.push(TokenTree::Brackets(from_tokens(stream)));
                assert_eq!(stream.next(), Some(Token::RightBracket));
            }
            Some(Token::LeftBrace) => {
                stream.next();
                result.push(TokenTree::Braces(from_tokens(stream)));
                assert_eq!(stream.next(), Some(Token::RightBrace));
            }
            Some(_) => result.push(TokenTree::Token(stream.next().unwrap())),
        }
    }
}

pub fn untreeify(token_tree: impl IntoIterator<Item = TokenTree>) -> Vec<Token> {
    let mut result = vec![];
    to_tokens(&mut result, token_tree);
    result
}

fn to_tokens(result: &mut Vec<Token>, token_tree: impl IntoIterator<Item = TokenTree>) {
    for token_tree in token_tree {
        match token_tree {
            TokenTree::Parenthesis(tree) => {
                result.push(Token::LeftParen);
                to_tokens(result, tree);
                result.push(Token::RightParen);
            }
            TokenTree::Brackets(tree) => {
                result.push(Token::LeftBracket);
                to_tokens(result, tree);
                result.push(Token::RightBracket);
            },
            TokenTree::Braces(tree) => {
                result.push(Token::LeftBrace);
                to_tokens(result, tree);
                result.push(Token::RightBrace);
            }
            TokenTree::Token(token) => result.push(token),
        }
    }
}
