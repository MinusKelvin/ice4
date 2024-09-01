use std::collections::{HashMap, HashSet};
use std::path::Path;

use once_cell::sync::Lazy;
use regex::Regex;

use crate::parse::{Lexer, Token};

static LOCAL_INCLUDE: Lazy<Regex> = Lazy::new(|| Regex::new(r#"#include "([^"]*)""#).unwrap());
static LIB_INCLUDE: Lazy<Regex> = Lazy::new(|| Regex::new(r#"#include <([^>]*)>"#).unwrap());
static PRAGMA: Lazy<Regex> = Lazy::new(|| Regex::new(r#"#pragma (\w+)"#).unwrap());
static DEFINE: Lazy<Regex> = Lazy::new(|| Regex::new(r#"#define (\w+) (.*)"#).unwrap());
static UNDEF: Lazy<Regex> = Lazy::new(|| Regex::new(r#"#undef (\w+)"#).unwrap());

#[derive(Default)]
pub struct Preprocessed {
    pub lib_includes: Vec<String>,
    pub code: Vec<Token>,
}

pub fn preprocess(path: &Path, tcec: bool, openbench: bool) -> Preprocessed {
    let mut result = Preprocessed::default();
    process(
        &mut result,
        &mut Lexer::new(),
        &mut HashSet::new(),
        &mut HashMap::new(),
        path,
        tcec,
        openbench,
    );
    result
}

fn process(
    into: &mut Preprocessed,
    lexer: &mut Lexer,
    onces: &mut HashSet<String>,
    defines: &mut HashMap<String, String>,
    path: &Path,
    tcec: bool,
    openbench: bool,
) {
    let content =
        std::fs::read_to_string(path).unwrap_or_else(|e| panic!("{e}: {}", path.display()));

    let mut lines = content.lines();
    let mut skip_else = false;
    while let Some(line) = lines.next() {
        if let Some(captures) = LOCAL_INCLUDE.captures(line) {
            let file = captures.get(1).unwrap().as_str();
            let path = path.parent().unwrap().join(file);
            // Check if the file has already been included.
            if !onces.contains(path.to_string_lossy().as_ref()) {
                process(into, lexer, onces, defines, &path, tcec, openbench);
            }
        } else if let Some(captures) = LIB_INCLUDE.captures(line) {
            let file = captures.get(1).unwrap().as_str();
            into.lib_includes.push(file.to_owned());
        } else if let Some(captures) = PRAGMA.captures(line) {
            let directive = captures.get(1).unwrap().as_str();
            if directive != "once" {
                eprintln!(
                    "Unknown pragma directive in {}: {directive}",
                    path.display()
                );
                continue;
            }
            onces.insert(path.to_string_lossy().into_owned());
        } else if let Some(captures) = DEFINE.captures(line) {
            let text = regex::escape(captures.get(1).unwrap().as_str());
            // TCEC builds use 96 GB hash and 101 threads
            let replacement = match &*text {
                "HASH_SIZE" if tcec => "0x300000000ull",
                "THREADS" if tcec => "101",
                _ => captures.get(2).unwrap().as_str(),
            };
            if let Some(old) = defines.insert(text.to_owned(), replacement.to_owned()) {
                if replacement != old {
                    eprintln!(
                        "'{text}' macro redefined in {}: '{old}' => '{replacement}'",
                        path.display()
                    );
                }
            }
        } else if let Some(captures) = UNDEF.captures(line) {
            defines.remove(captures.get(1).unwrap().as_str());
        } else if !openbench && line == "#ifdef OPENBENCH" || line == "#ifdef AVOID_ADJUDICATION" {
            // munch until end of block
            while !matches!(lines.next(), Some("#endif" | "#else")) {}
        } else if openbench && line == "#ifdef OPENBENCH" {
            skip_else = true;
        } else if skip_else && line == "#else" {
            // munch until end of block
            while !matches!(lines.next(), Some("#endif")) {}
            skip_else = false;
        } else if line == "#endif" {
            // ignore, probably just the end of the #else of above case
        } else if line.starts_with("#define S") {
            // ignore, we'll hack this back in later
        } else if line.starts_with('#') {
            panic!(
                "Unrecognized preprocessor directive in {}: {line}",
                path.display()
            );
        } else {
            let line_tokens = lexer.tokenize(line);
            replace_stream(&mut into.code, lexer, defines, line_tokens.into_iter())
        }
    }
}

fn replace_stream(
    into: &mut Vec<Token>,
    lexer: &mut Lexer,
    defines: &HashMap<String, String>,
    stream: impl Iterator<Item = Token>,
) {
    let mut stream = stream.peekable();
    while let Some(token) = stream.next() {
        if let Some("S") = token.word() {
            let Some(Token::LeftParen) = stream.next() else {
                panic!("S macro requires arguments");
            };

            let sign_1 = match stream.next_if_eq(&Token::Minus).is_some() {
                true => -1,
                false => 1,
            };
            let Some(Token::Integer(number_1)) = stream.next() else {
                panic!("S macro requires integers")
            };

            let Some(Token::Comma) = stream.next() else {
                panic!("S macro requires comma after first number")
            };

            let sign_2 = match stream.next_if_eq(&Token::Minus).is_some() {
                true => -1,
                false => 1,
            };
            let Some(Token::Integer(number_2)) = stream.next() else {
                panic!("S macro requires integers")
            };

            let Some(Token::RightParen) = stream.next() else {
                panic!("S macro requires closing parenthesis");
            };

            let mg = sign_1 * number_1.value as i32;
            let eg = sign_2 * number_2.value as i32;

            into.extend(lexer.tokenize(&format!("{}", mg + (eg << 16))));
        } else if let Some(replacement) = token.word().and_then(|word| defines.get(word)) {
            let replacement_tokens = lexer.tokenize(replacement);
            replace_stream(into, lexer, defines, replacement_tokens.into_iter());
        } else {
            into.push(token);
        }
    }
}
