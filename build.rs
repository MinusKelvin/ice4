fn main() {
    lalrpop::process_root().unwrap();
    println!("cargo:rerun-if-changed=minifier/parse/cpp.lalrpop");
}
