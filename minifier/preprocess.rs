use std::borrow::Cow;
use std::path::Path;

use once_cell::sync::Lazy;
use regex::{NoExpand, Regex};

static LOCAL_INCLUDE: Lazy<Regex> = Lazy::new(|| Regex::new(r#"#include "([^"]*)""#).unwrap());
static LIB_INCLUDE: Lazy<Regex> = Lazy::new(|| Regex::new(r#"#include <([^>]*)>"#).unwrap());
static DEFINE: Lazy<Regex> = Lazy::new(|| Regex::new(r#"#define (\w+) (.*)"#).unwrap());

static S_EVAL: Lazy<Regex> = Lazy::new(|| Regex::new(r#"S\(([-0-9]+),\s+([-0-9]+)\)"#).unwrap());

#[derive(Default)]
pub struct Preprocessed {
    pub lib_includes: Vec<String>,
    pub code: String,
}

pub fn preprocess(path: &Path, tcec: bool) -> Preprocessed {
    let mut result = Preprocessed::default();
    process(&mut result, &mut vec![], path, tcec);
    result
}

fn process(into: &mut Preprocessed, defines: &mut Vec<(Regex, String)>, path: &Path, tcec: bool) {
    let content =
        std::fs::read_to_string(path).unwrap_or_else(|e| panic!("{e}: {}", path.display()));

    let mut lines = content.lines();
    while let Some(line) = lines.next() {
        if let Some(captures) = LOCAL_INCLUDE.captures(line) {
            let file = captures.get(1).unwrap().as_str();
            let path = path.parent().unwrap().join(file);
            process(into, defines, &path, tcec);
        } else if let Some(captures) = LIB_INCLUDE.captures(line) {
            let file = captures.get(1).unwrap().as_str();
            into.lib_includes.push(file.to_owned());
        } else if let Some(captures) = DEFINE.captures(line) {
            let text = regex::escape(captures.get(1).unwrap().as_str());
            let mut replacement = " ".to_owned();
            // TCEC builds use 64 GB hash and 101 threads
            match &*text {
                "HASH_SIZE" if tcec => replacement.push_str("0x100000000ull"),
                "THREADS" if tcec => replacement.push_str("101"),
                _ => replacement.push_str(captures.get(2).unwrap().as_str()),
            }
            replacement.push(' ');
            defines.push((Regex::new(&format!("\\b{text}\\b")).unwrap(), replacement));
        } else if line == "#ifdef OPENBENCH" || line == "#ifdef AVOID_ADJUDICATION" {
            // munch until end of block
            while !matches!(lines.next(), Some("#endif" | "#else")) {}
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
            let mut line_replaced = line.to_owned();
            for (pattern, replacement) in defines.iter().rev() {
                if let Cow::Owned(s) = pattern.replace_all(&line_replaced, NoExpand(replacement)) {
                    line_replaced = s;
                }
            }

            while let Some(captures) = S_EVAL.captures(&line_replaced) {
                let mg: i32 = captures.get(1).unwrap().as_str().parse().unwrap();
                let eg: i32 = captures.get(2).unwrap().as_str().parse().unwrap();
                line_replaced.replace_range(
                    captures.get(0).unwrap().range(),
                    &format!("{}", mg + eg << 16),
                );
            }

            into.code.push_str(&line_replaced);
            into.code.push('\n');
        }
    }
}
