mod parse;
mod preprocess;
mod renamer;
mod decl_merger;

fn main() {
    let preprocessed = preprocess::preprocess("src/main.cpp".as_ref());
    eprintln!("Raw size: {}", preprocessed.code.len());

    let mut ast = parse::parse(&preprocessed.code);

    decl_merger::merge_decls(&mut ast);

    let mut tokens = parse::unparse(ast);

    renamer::rename_identifiers(&mut tokens);

    let packed = parse::stringify(tokens);

    eprintln!("Minified size: {}", packed.len());
    for lib in &preprocessed.lib_includes {
        println!("#include<{lib}>");
    }
    print!("{packed}");
}
