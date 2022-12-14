use std::borrow::Cow;
use std::collections::HashSet;

use once_cell::sync::Lazy;
use regex::Regex;

static KEYWORDS: Lazy<HashSet<&'static str>> =
    Lazy::new(|| include_str!("keywords").lines().collect());

#[derive(Clone, Debug, PartialEq, Eq)]
pub enum Token {
    Identifier(String),
    Typename(String),
    Keyword(&'static str),
    Integer(ParsedNumber),
    OtherNumber(String),
    String(String),
    Ampersand,
    AmpersandAmpersand,
    AmpersandEqual,
    Bang,
    BangEqual,
    Bar,
    BarBar,
    BarEqual,
    Caret,
    CaretEqual,
    Colon,
    ColonColon,
    Comma,
    Dot,
    DotDotDot,
    Equal,
    EqualEqual,
    Greater,
    GreaterEqual,
    GreaterGreater,
    GreaterGreaterEqual,
    LeftBrace,
    LeftBracket,
    LeftParen,
    Less,
    LessEqual,
    LessLess,
    LessLessEqual,
    Minus,
    MinusEqual,
    MinusGreater,
    MinusMinus,
    Percent,
    PercentEqual,
    Plus,
    PlusEqual,
    PlusPlus,
    Question,
    RightBrace,
    RightBracket,
    RightParen,
    Semicolon,
    Slash,
    SlashEqual,
    Star,
    StarEqual,
    Tilde,
}

#[derive(Clone, Debug, PartialEq, Eq)]
pub struct ParsedNumber {
    value: u64,
    suffix: String,
}

impl ParsedNumber {
    pub fn to_string(&self) -> String {
        let decimal = format!("{}{}", self.value, self.suffix);
        let hex = format!("0x{:x}{}", self.value, self.suffix);
        if decimal.len() <= hex.len() {
            decimal
        } else {
            hex
        }
    }
}

impl Token {
    pub fn as_str(&self) -> Cow<str> {
        match self {
            Token::Identifier(w) => w,
            Token::Typename(w) => w,
            Token::Keyword(k) => *k,
            Token::Integer(n) => return Cow::Owned(n.to_string()),
            Token::OtherNumber(n) => n,
            Token::String(s) => s,
            Token::Ampersand => "&",
            Token::AmpersandAmpersand => "&&",
            Token::AmpersandEqual => "&=",
            Token::Bang => "!",
            Token::BangEqual => "!=",
            Token::Bar => "|",
            Token::BarBar => "||",
            Token::BarEqual => "|=",
            Token::Caret => "^",
            Token::CaretEqual => "^=",
            Token::Colon => ":",
            Token::ColonColon => "::",
            Token::Comma => ",",
            Token::Dot => ".",
            Token::DotDotDot => "...",
            Token::Equal => "=",
            Token::EqualEqual => "==",
            Token::Greater => ">",
            Token::GreaterEqual => ">=",
            Token::GreaterGreater => ">>",
            Token::GreaterGreaterEqual => ">>=",
            Token::LeftBrace => "{",
            Token::LeftBracket => "[",
            Token::LeftParen => "(",
            Token::Less => "<",
            Token::LessEqual => "<=",
            Token::LessLess => "<<",
            Token::LessLessEqual => "<<=",
            Token::Minus => "-",
            Token::MinusEqual => "-=",
            Token::MinusGreater => "->",
            Token::MinusMinus => "--",
            Token::Percent => "%",
            Token::PercentEqual => "%=",
            Token::Plus => "+",
            Token::PlusEqual => "+=",
            Token::PlusPlus => "++",
            Token::Question => "?",
            Token::RightBrace => "}",
            Token::RightBracket => "]",
            Token::RightParen => ")",
            Token::Semicolon => ";",
            Token::Slash => "/",
            Token::SlashEqual => "/=",
            Token::Star => "*",
            Token::StarEqual => "*=",
            Token::Tilde => "~",
        }
        .into()
    }

    fn is_wordlike(&self) -> bool {
        matches!(
            self,
            Token::Identifier(_)
                | Token::Keyword(_)
                | Token::Typename(_)
                | Token::Integer(_)
                | Token::OtherNumber(_)
        )
    }

    pub fn requires_space(&self, other: &Token) -> bool {
        match (self, other) {
            (l, r) if l.is_wordlike() && r.is_wordlike() => true,
            (Token::Integer(_), Token::Dot | Token::DotDotDot) => true,
            (Token::Dot | Token::DotDotDot, Token::Integer(_)) => true,
            (l, _) if l.is_wordlike() => false,
            (_, r) if r.is_wordlike() => false,
            _ => {
                let buf = self.as_str().to_owned() + other.as_str();
                tokenize(&buf).get(0) != Some(self)
            }
        }
    }
}

macro_rules! match_prefix {
    ($expr:expr;
        $($prefix:literal $(| $alt:literal)*, $tail:ident => $body:expr,)*
        _ => $default_body:expr $(,)?
    ) => {{
        let s = $expr;
        if false {
            unreachable!()
        } $(else if let Some($tail) = s.strip_prefix($prefix) $(.or(s.strip_prefix($alt)))* {
            $body
        })* else {
            $default_body
        }
    }}
}

static WORD: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A[a-zA-Z_][a-zA-Z0-9_]*"#).unwrap());
static NUMBER: Lazy<Regex> =
    Lazy::new(|| Regex::new(r#"\A[0-9][a-zA-Z0-9']*(\.[a-zA-Z0-9']+)?"#).unwrap());
static STRING: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A"([^\\"]|\\.)*""#).unwrap());
static CHARACTER: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A'([^\\']|\\.)*'"#).unwrap());

pub fn tokenize(mut text: &str) -> Vec<Token> {
    let mut typenames: HashSet<_> = include_str!("extern_types")
        .lines()
        .map(|s| s.to_owned())
        .collect();

    let mut result = vec![];
    while !text.is_empty() {
        let type_introduction = matches!(
            result.last(),
            Some(Token::Keyword("struct" | "class" | "union" | "enum"))
        );
        let code = text;
        let mut token = |token, rest| {
            result.push(token);
            text = rest;
        };
        match_prefix! { code;
            " " | "\t" | "\n", rest => text = rest,
            "&&", rest => token(Token::AmpersandAmpersand, rest),
            "&=", rest => token(Token::AmpersandEqual, rest),
            "&", rest => token(Token::Ampersand, rest),
            "!=", rest => token(Token::BangEqual, rest),
            "!", rest => token(Token::Bang, rest),
            "||", rest => token(Token::BarBar, rest),
            "|=", rest => token(Token::BarEqual, rest),
            "|", rest => token(Token::Bar, rest),
            "^=", rest => token(Token::CaretEqual, rest),
            "^", rest => token(Token::Caret, rest),
            "::", rest => token(Token::ColonColon, rest),
            ":", rest => token(Token::Colon, rest),
            ",", rest => token(Token::Comma, rest),
            "...", rest => token(Token::DotDotDot, rest),
            ".", rest => token(Token::Dot, rest),
            "==", rest => token(Token::EqualEqual, rest),
            "=", rest => token(Token::Equal, rest),
            ">>=", rest => token(Token::GreaterGreaterEqual, rest),
            ">=", rest => token(Token::GreaterEqual, rest),
            ">>", rest => token(Token::GreaterGreater, rest),
            ">", rest => token(Token::Greater, rest),
            "{", rest => token(Token::LeftBrace, rest),
            "[", rest => token(Token::LeftBracket, rest),
            "(", rest => token(Token::LeftParen, rest),
            "<<=", rest => token(Token::LessLessEqual, rest),
            "<=", rest => token(Token::LessEqual, rest),
            "<<", rest => token(Token::LessLess, rest),
            "<", rest => token(Token::Less, rest),
            "-=", rest => token(Token::MinusEqual, rest),
            "->", rest => token(Token::MinusGreater, rest),
            "--", rest => token(Token::MinusMinus, rest),
            "-", rest => token(Token::Minus, rest),
            "%=", rest => token(Token::PercentEqual, rest),
            "%", rest => token(Token::Percent, rest),
            "+=", rest => token(Token::PlusEqual, rest),
            "++", rest => token(Token::PlusPlus, rest),
            "+", rest => token(Token::Plus, rest),
            "?", rest => token(Token::Question, rest),
            "}", rest => token(Token::RightBrace, rest),
            "]", rest => token(Token::RightBracket, rest),
            ")", rest => token(Token::RightParen, rest),
            ";", rest => token(Token::Semicolon, rest),
            "//", rest => {
                text = rest.split_once('\n').map(|(_, v)| v).unwrap_or("");
            },
            "/*", rest => {
                text = rest.split_once("*/").map(|(_, v)| v).unwrap_or("");
            },
            "/=", rest => token(Token::SlashEqual, rest),
            "/", rest => token(Token::Slash, rest),
            "*", rest => token(Token::Star, rest),
            "*=", rest => token(Token::StarEqual, rest),
            "~", rest => token(Token::Tilde, rest),
            _ => if let Some(m) = WORD.find(code) {
                let tok = if let Some(k) = KEYWORDS.get(m.as_str()) {
                    Token::Keyword(k)
                } else if type_introduction {
                    typenames.insert(m.as_str().to_owned());
                    Token::Typename(m.as_str().to_owned())
                } else if typenames.contains(m.as_str()) {
                    Token::Typename(m.as_str().to_owned())
                } else {
                    Token::Identifier(m.as_str().to_owned())
                };
                token(tok, &code[m.end()..]);
            } else if let Some(m) = NUMBER.find(code).or_else(|| CHARACTER.find(code)) {
                token(parse_number(m.as_str()), &code[m.end()..]);
            } else if let Some(m) = STRING.find(code) {
                token(Token::String(m.as_str().to_owned()), &code[m.end()..]);
            } else {
                unreachable!(
                    "could not parse token starting with `{}`",
                    &code[..code.len().min(8)]
                );
            },
        }
    }
    string_concat(&mut result);
    result
}

const BINARY: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A0b([0-1]+)(\w*)\z"#).unwrap());
const OCTAL: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A(0[0-7]*)(\w*)\z"#).unwrap());
const DECIMAL: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A([1-9][0-9]*)(\w*)\z"#).unwrap());
const HEX: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A0x([0-9A-Fa-f]+)(\w*)\z"#).unwrap());
const CHAR: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A'(([^'\\]|\\.)+)'\z"#).unwrap());

fn parse_number(text: &str) -> Token {
    Token::Integer(if let Some(captures) = OCTAL.captures(text) {
        ParsedNumber {
            value: u64::from_str_radix(captures.get(1).unwrap().as_str(), 8).unwrap(),
            suffix: captures.get(2).unwrap().as_str().to_owned(),
        }
    } else if let Some(captures) = DECIMAL.captures(text) {
        ParsedNumber {
            value: u64::from_str_radix(captures.get(1).unwrap().as_str(), 10).unwrap(),
            suffix: captures.get(2).unwrap().as_str().to_owned(),
        }
    } else if let Some(captures) = HEX.captures(text) {
        ParsedNumber {
            value: u64::from_str_radix(captures.get(1).unwrap().as_str(), 16).unwrap(),
            suffix: captures.get(2).unwrap().as_str().to_owned(),
        }
    } else if let Some(captures) = BINARY.captures(text) {
        ParsedNumber {
            value: u64::from_str_radix(captures.get(1).unwrap().as_str(), 2).unwrap(),
            suffix: captures.get(2).unwrap().as_str().to_owned(),
        }
    } else if let Some(captures) = CHAR.captures(text) {
        let mut value = 0;
        let mut iter = captures.get(1).unwrap().as_str().chars();
        loop {
            let char_value = match iter.next() {
                Some('\\') => match iter.next().unwrap() {
                    'a' => 0x07,
                    'b' => 0x08,
                    'e' => 0x1B,
                    'f' => 0x0C,
                    'n' => 0x0A,
                    'r' => 0x0D,
                    't' => 0x09,
                    'v' => 0x0B,
                    '\\' => 0x5C,
                    '\'' => 0x27,
                    '"' => 0x22,
                    '?' => 0x3F,
                    c => panic!("invalid escape sequence: `\\{c}`"),
                },
                Some(c) => c as u64,
                None => break,
            };
            value <<= 8;
            value |= char_value;
        }
        ParsedNumber {
            value,
            suffix: String::new(),
        }
    } else {
        return Token::OtherNumber(text.to_string());
    })
}

fn string_concat(tokens: &mut Vec<Token>) {
    tokens.dedup_by(|r, l| match (l, r) {
        (Token::String(l), Token::String(r)) => {
            l.pop(); // remove closing "
            l.push_str(&r[1..]); // remove opening "
            true
        }
        _ => false,
    })
}
