use std::collections::{HashMap, HashSet};

use crate::lexical::Token;

pub fn rename_identifiers(tokens: &mut [Token]) {
    let keywords: HashSet<_> = include_str!("keywords").lines().collect();

    let mut counts: HashMap<_, usize> = HashMap::new();

    for token in &*tokens {
        if let Token::Word(word) = token {
            if keywords.contains(&**word) {
                continue;
            }

            *counts.entry(word).or_default() += 1;
        }
    }

    let mut words: Vec<_> = counts.into_iter().collect();
    words.sort_by_key(|&(w, c)| (std::cmp::Reverse(c), w));

    let mut translation_table = HashMap::new();
    let mut ident_state = vec![11];
    for (word, _) in words {
        let mut ident = String::new();
        for &i in ident_state.iter() {
            ident.push(IDENT_CHARACTERS[i..].chars().next().unwrap());
        }
        translation_table.insert(word.clone(), ident);
        for i in ident_state.iter_mut() {
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

    // dbg!(&translation_table);

    for token in tokens {
        if let Token::Word(word) = token {
            if keywords.contains(&**word) {
                continue;
            }
            *word = translation_table.get(word).unwrap().clone();
        }
    }
}

const IDENT_CHARACTERS: &str = "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";
