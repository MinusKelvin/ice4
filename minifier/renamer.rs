use std::collections::HashMap;

use crate::parse::Token;
use crate::semantic_analysis::Symbols;

pub fn rename_identifiers(symbols: &Symbols, tokens: &mut [Token]) {
    let mut coloring = vec![None; symbols.symbols.len()];
    let mut counts = vec![];

    loop {
        let mut could_become_member: Vec<_> = coloring
            .iter()
            .map(|c: &Option<usize>| c.is_none())
            .collect();

        if coloring.iter().all(|&c| c.is_some()) {
            break;
        }

        let next_id = counts.len();
        counts.push(0);

        loop {
            let next_addition = could_become_member
                .iter()
                .enumerate()
                .filter(|&(_, &allowed)| allowed)
                .max_by_key(|&(id, _)| symbols.symbols[id].occurances);

            let next_addition = match next_addition {
                Some((id, _)) => id,
                None => break,
            };

            coloring[next_addition] = Some(next_id);
            could_become_member[next_addition] = false;
            for &adj in &symbols.symbols[next_addition].others_in_scope {
                could_become_member[adj] = false;
            }
            counts[next_id] += symbols.symbols[next_addition].occurances;
        }
    }

    let names = generate_variable_names(counts.len());
    let translation_table: HashMap<_, _> = coloring
        .into_iter()
        .enumerate()
        .map(|(id, colored)| (format!("${id}"), &names[colored.unwrap()]))
        .collect();

    for token in tokens {
        match token {
            Token::Identifier(name) | Token::Typename(name) => {
                if let Some(&new) = translation_table.get(name) {
                    *name = new.clone();
                }
            }
            _ => {}
        }
    }
}

const IDENT_CHARACTERS: &str = "0123456789_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

pub fn generate_variable_names(count: usize) -> Vec<String> {
    let mut idents = vec![];
    let mut ident_state = vec![11];
    for _ in 0..count {
        let mut ident = String::new();
        for &i in ident_state.iter() {
            ident.push(IDENT_CHARACTERS[i..].chars().next().unwrap());
        }
        idents.push(ident);
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
    idents
}
