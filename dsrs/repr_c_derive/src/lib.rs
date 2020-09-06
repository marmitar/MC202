#![feature(unsafe_block_in_unsafe_fn)]
#![feature(iterator_fold_self)]
#![feature(try_trait)]
#![deny(unsafe_op_in_unsafe_fn)]

extern crate proc_macro;
mod check;
mod derive;

use proc_macro::TokenStream;
use syn::parse_macro_input;
use derive::repr_c;


#[proc_macro_derive(ReprC)]
pub fn rep_c_derive(input: TokenStream) -> TokenStream {
    let derive = parse_macro_input!(input);
    repr_c(derive, true).into()
}

#[proc_macro_derive(Unsafe_ReprC)]
pub fn repr_c_unsafe_derive(input: TokenStream) -> TokenStream {
    let derive = parse_macro_input!(input);
    repr_c(derive, false).into()
}
