#![feature(const_option)]
#![warn(clippy::all, clippy::pedantic, clippy::nursery, clippy::cargo)]
#![allow(clippy::cargo_common_metadata)]
#![allow(clippy::multiple_crate_versions)]
#![allow(clippy::wildcard_dependencies)]
extern crate proc_macro;

mod check;
mod derive;

use proc_macro::TokenStream;

use check::check_attributes;
use derive::impl_repr_c;

#[proc_macro_derive(ReprC)]
pub fn repr_c_derive(input: TokenStream) -> TokenStream {

    match impl_repr_c(input.into(), check_attributes) {
        Ok(derive) => derive,
        Err(err) => err.to_compile_error(),
    }
    .into()
}

#[proc_macro_derive(Unsafe_ReprC)]
pub fn repr_c_unsafe_derive(input: TokenStream) -> TokenStream {
    let skip_check = |_| Ok(());

    match impl_repr_c(input.into(), skip_check) {
        Ok(derive) => derive,
        Err(err) => err.to_compile_error(),
    }
    .into()
}
