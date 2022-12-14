use std::collections::{HashMap, HashSet};

use crate::parse::Token;

pub fn rename_identifiers(tokens: &mut [Token]) {
    let unrenameable: HashSet<_> = include_str!("extern_idents")
        .lines()
        .chain(include_str!("parse/extern_types").lines())
        .collect();

    let mut counts = HashMap::new();

    for token in &*tokens {
        if let Token::Identifier(word) | Token::Typename(word) = token {
            if unrenameable.contains(&**word) {
                continue;
            }

            *counts.entry(&**word).or_default() += 1;
        }
    }

    // dbg!(&counts, counts.len());

    let translation_table = make_translation_table(counts);

    for token in tokens {
        if let Token::Identifier(word) | Token::Typename(word) = token {
            if unrenameable.contains(&**word) {
                continue;
            }
            *word = translation_table.get(word).unwrap().clone();
        }
    }
}

const IDENT_CHARACTERS: &str = "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

pub fn make_translation_table(counts: HashMap<&str, usize>) -> HashMap<String, String> {
    let mut words: Vec<_> = counts.into_iter().collect();
    words.sort_by_key(|&(w, c)| (std::cmp::Reverse(c), w));

    let mut translation_table = HashMap::new();
    let mut ident_state = vec![11];
    for (word, _) in words {
        let mut ident = String::new();
        for &i in ident_state.iter() {
            ident.push(IDENT_CHARACTERS[i..].chars().next().unwrap());
        }
        translation_table.insert(word.to_owned(), ident);
        for i in ident_state.iter_mut().rev() {
            *i += 1;
            if *i == IDENT_CHARACTERS.len() {
                *i = 0;
            } else {
                break;
            }
        }
        if ident_state[0] == 0 {
            ident_state[0] = 11;
            ident_state.push(0);
        }
    }
    translation_table
}
