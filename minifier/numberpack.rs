use once_cell::sync::Lazy;
use regex::Regex;

use crate::lexical::Token;

struct ParsedNumber<'a> {
    value: u64,
    suffix: &'a str,
}

impl ParsedNumber<'_> {
    fn decimal(&self) -> String {
        format!("{}{}", self.value, self.suffix).to_ascii_lowercase()
    }

    fn hex(&self) -> String {
        format!("0x{:x}{}", self.value, self.suffix).to_ascii_lowercase()
    }
}

pub fn minify_numbers(tokens: &mut [Token]) {
    for token in tokens {
        if let Token::Number(word) = token {
            if let Some(parsed) = parse_number(word) {
                let decimal = parsed.decimal();
                let hex = parsed.hex();
                if decimal.len() <= hex.len() {
                    *word = decimal;
                } else {
                    *word = hex;
                }
            }
        }
    }
}

const BINARY: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A0b([0-1]+)(\w*)\z"#).unwrap());
const OCTAL: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A(0[0-7]*)(\w*)\z"#).unwrap());
const DECIMAL: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A([1-9][0-9]*)(\w*)\z"#).unwrap());
const HEX: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A0x([0-9A-Fa-f]+)(\w*)\z"#).unwrap());
const CHAR: Lazy<Regex> = Lazy::new(|| Regex::new(r#"\A'(([^'\\]|\\.)+)'\z"#).unwrap());

fn parse_number(text: &str) -> Option<ParsedNumber> {
    Some(if let Some(captures) = OCTAL.captures(text) {
        ParsedNumber {
            value: u64::from_str_radix(captures.get(1).unwrap().as_str(), 8).unwrap(),
            suffix: captures.get(2).unwrap().as_str(),
        }
    } else if let Some(captures) = DECIMAL.captures(text) {
        ParsedNumber {
            value: u64::from_str_radix(captures.get(1).unwrap().as_str(), 10).unwrap(),
            suffix: captures.get(2).unwrap().as_str(),
        }
    } else if let Some(captures) = HEX.captures(text) {
        ParsedNumber {
            value: u64::from_str_radix(captures.get(1).unwrap().as_str(), 16).unwrap(),
            suffix: captures.get(2).unwrap().as_str(),
        }
    } else if let Some(captures) = BINARY.captures(text) {
        ParsedNumber {
            value: u64::from_str_radix(captures.get(1).unwrap().as_str(), 2).unwrap(),
            suffix: captures.get(2).unwrap().as_str(),
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
        ParsedNumber { value, suffix: "" }
    } else {
        return None;
    })
}
