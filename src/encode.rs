use std::env;

fn main() {
    let args: Vec<String> = env::args().collect();

    if args.len() != 4 {

    }
    dbg!(args);
    println!("Hello, world!");
}
