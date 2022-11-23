mod lexical;
mod preprocess;
mod renamer;

fn main() {
    let preprocessed = preprocess::preprocess("src/main.cpp".as_ref());
    eprintln!("Raw size: {}", preprocessed.code.len());
    let mut tokens = lexical::tokenize(&preprocessed.code);

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
