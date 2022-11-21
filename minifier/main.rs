use lexical::Token;

mod lexical;
mod numberpack;
mod preprocess;
mod renamer;

fn main() {
    let preprocessed = preprocess::preprocess("src/main.cpp".as_ref());
    eprintln!("Raw size: {}", preprocessed.code.len());
    let mut tokens = lexical::tokenize(&preprocessed.code);

    renamer::rename_identifiers(&mut tokens);
    numberpack::minify_numbers(&mut tokens);
    string_concat(&mut tokens);

    let mut packed = String::new();
    for pair in tokens.windows(2) {
        packed.push_str(pair[0].as_str());
        if pair[0].requires_space(&pair[1]) {
            packed.push(' ');
        }
    }
    packed.push_str(tokens.last().unwrap().as_str());
    eprintln!("Minified size: {}", packed.len());
    for lib in &preprocessed.lib_includes {
        println!("#include <{lib}>");
    }
    print!("{packed}");
}

fn string_concat(tokens: &mut Vec<Token>) {
    tokens.dedup_by(|r, l| match (l, r) {
        (Token::String(l), Token::String(r)) => {
            l.pop(); // remove closing "
            l.push_str(&r[1..]); // remove opening "
            true
        },
        _ => false,
    })
}
