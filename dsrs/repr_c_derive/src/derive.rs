use proc_macro2::TokenStream;
use syn::DeriveInput;
// use quote::quote;

use crate::check::check_attributes;


pub fn repr_c(input: DeriveInput, check: bool) -> TokenStream {
    // TODO: check after?
    if check {
        // TODO: something else will return Results down there
        if let Err(err) = check_attributes(input.attrs) {
            return err.to_compile_error()
        }
    }

    todo!();
}
