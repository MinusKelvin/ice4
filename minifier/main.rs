mod ast;
mod lexical;
mod preprocess;
mod renamer;

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
            .map(|(i, t)| Ok((i, t, i+1))),
    );
    dbg!(parsed);

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
