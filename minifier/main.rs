mod ast;
mod lexical;
mod preprocess;
mod renamer;
mod tokenize;

mod cpp;

fn main() {
    let preprocessed = preprocess::preprocess("src/main.cpp".as_ref());
    eprintln!("Raw size: {}", preprocessed.code.len());
    let mut tokens = lexical::tokenize(&preprocessed.code);

    let parser = cpp::FileParser::new();
    let parsed = parser.parse(
        tokens
            .clone()
            .into_iter()
            .enumerate()
            .map(|(i, t)| Ok((i, t, i + 1))),
    );
    match parsed {
        Ok(ast) => {
            let tokens = tokenize::tokenize(ast);
            let mut formatted = String::new();
            let mut indentation_level = 0;
            let mut newline = false;
            for t in tokens {
                if matches!(t, lexical::Token::RightBrace) {
                    indentation_level -= 1;
                    newline = true;
                }
                if newline {
                    formatted.push('\n');
                    for _ in 0..indentation_level {
                        formatted.push_str("    ");
                    }
                    newline = false;
                }
                formatted.push_str(&t.as_str());
                formatted.push(' ');
                if matches!(t, lexical::Token::LeftBrace) {
                    indentation_level += 1;
                    newline = true;
                }
                if matches!(t, lexical::Token::Semicolon | lexical::Token::RightBrace) {
                    newline = true;
                }
            }
            eprintln!("{formatted}");
        }
        Err(lalrpop_util::ParseError::UnrecognizedToken { token, .. }) => {
            let mut packed = String::new();
            for (i, t) in tokens
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
        }
        _ => {}
    }

    renamer::rename_identifiers(&mut tokens);

    let mut packed = String::new();
    for pair in tokens.windows(2) {
        packed.push_str(&pair[0].as_str());
        if pair[0].requires_space(&pair[1]) {
            packed.push(' ');
        }
    }
    packed.push_str(&tokens.last().unwrap().as_str());
    eprintln!("Minified size: {}", packed.len());
    for lib in &preprocessed.lib_includes {
        println!("#include<{lib}>");
    }
    print!("{packed}");
}
