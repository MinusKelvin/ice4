use std::collections::HashMap;

use crate::parse::Token;
use crate::semantic_analysis::Symbols;

pub fn rename_identifiers(symbols: &Symbols, tokens: &mut [Token]) {
    let mut coloring = vec![None; symbols.symbols.len()];
    let mut counts = vec![];

    loop {
        if coloring.iter().all(|&c| c.is_some()) {
            break;
        }

        let (size, colored) = group(
            symbols,
            &mut coloring
                .iter()
                .map(|c: &Option<usize>| c.is_some() as usize)
                .collect::<Vec<_>>(),
        );

        for id in colored {
            coloring[id] = Some(counts.len());
        }
        counts.push(size);
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

fn group(symbols: &Symbols, blockers: &mut [usize]) -> (usize, Vec<usize>) {
    let addition = blockers
        .iter()
        .enumerate()
        .filter(|&(_, &blockers)| blockers == 0)
        .max_by_key(|&(id, _)| symbols.symbols[id].occurances);

    let id1 = match addition {
        Some((id, _)) => id,
        None => return (0, vec![]),
    };

    blockers[id1] += 1;
    for &adj in &symbols.symbols[id1].others_in_scope {
        blockers[adj] += 1;
    }

    let (s, mut coloring1) = group(symbols, blockers);
    let size1 = s + symbols.symbols[id1].occurances;
    coloring1.push(id1);

    blockers[id1] -= 1;
    for &adj in &symbols.symbols[id1].others_in_scope {
        blockers[adj] -= 1;
    }

    let id2 = symbols.symbols[id1]
        .others_in_scope
        .iter()
        .copied()
        .filter(|&id| blockers[id] == 0)
        .max_by_key(|&id| symbols.symbols[id].occurances);

    if let Some(id2) = id2 {
        blockers[id2] += 1;
        for &adj in &symbols.symbols[id2].others_in_scope {
            blockers[adj] += 1;
        }

        let (s, mut coloring2) = group(symbols, blockers);
        let size2 = s + symbols.symbols[id2].occurances;
        coloring2.push(id2);

        blockers[id2] -= 1;
        for &adj in &symbols.symbols[id2].others_in_scope {
            blockers[adj] -= 1;
        }

        if size2 > size1 {
            return (size2, coloring2);
        }
    }

    (size1, coloring1)
}

const IDENT_CHARACTERS: &str = "_abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ";

pub fn generate_variable_names(count: usize) -> Vec<String> {
    let mut idents = vec![];
    let mut ident_state = vec![1];
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
            ident_state[0] = 1;
            ident_state.push(0);
        }
    }
    idents
}
