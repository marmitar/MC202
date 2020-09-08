//! Macro for checking and implementing [`ReprC`](mem::alloc::ReprC) trait.
use proc_macro2::{Span, TokenStream};
use std::fmt::Display;
use syn::Data::{Enum, Struct, Union};
use syn::{Attribute, DeriveInput, Error, Result};

use quote::quote;

/// Automatic trait to mark functions that can check attributes in
/// [`DeriveInput`](DeriveInput#fields).
///
/// An [`AttrChecker`] function is responsible for checking the
/// attributes of an input struct to guarantee that it follows
/// a `#[repr(C)]` layout.
///
/// All other `repr` hints should cause `Err`. But take care, as
/// repeated `C` hints are allowed in Rust. This should also `Err`
/// if `#[repr(C)]` or equivalent is missing in attribute
/// list.
///
/// # Note
///
/// This can't checke wether the input is a `struct` or not,
/// but it should assume so.
pub trait AttrChecker: FnOnce(Vec<Attribute>) -> Result<()> {}
impl<F: FnOnce(Vec<Attribute>) -> Result<()>> AttrChecker for F {}

/// Implement [`ReprC`](mem::alloc::ReprC) for the struct in input
/// `TokenStream`.
///
/// The [`attr_check`](AttrChecker) function should check the validity of the
/// struct attributes, specially the `#[repr(...)]` attributes, as the
/// `ReprC` trait can only be safely implemented on `#[repr(C)]` structs.
///
/// # Errors
///
/// This function will cause errors if `input` can't be parsed into a
/// [`DeriveInput`] or if the type implementing `ReprC` isn't a
/// [`struct`](Struct), as `enum`s and `union`s can't impl the trait.
///
/// This function will also return any errors encontered by `attr_check`.
#[inline]
pub fn impl_repr_c(input: TokenStream, attr_check: impl AttrChecker) -> Result<TokenStream> {

    let DeriveInput { attrs, vis: _, ident: name, generics, data } = syn::parse2(input)?;

    // get the type for the FieldTuple
    let field = match data {
        Struct(ref data) => data.fields.iter().map(|field| &field.ty),
        Enum(_) => return Err(not_a_struct(&name, "enum")),
        Union(_) => return Err(not_a_struct(&name, "union")),
    };
    // check after getting the fields, as that is a more important error
    attr_check(attrs)?;

    // the implementation is actually simple
    let (impl_generics, ty_generics, where_clause) = generics.split_for_impl();
    Ok(quote! {
        unsafe impl #impl_generics ::mem::alloc::ReprC for #name #ty_generics #where_clause {
            type Fields = (#(#field,)*);
        }
    })
}

/// Generate standard error for when given type is not a struct.
///
/// `ident` should be the struct name and `data_type` is a
/// string describing the kind of the type (`enum` or `union`).
fn not_a_struct<T: Display + ?Sized>(ident: &T, data_type: &str) -> Error {
    let message = format!(
        "wrong type layout: 'ReprC' can only be \
        implemented for structs, but {} is an {}",
        ident, data_type
    );

    Error::new(Span::call_site(), message)
}

#[cfg(test)]
mod tests {
    use super::{impl_repr_c, not_a_struct};

    use crate::check::check_attributes;
    use proc_macro2::TokenStream;
    use quote::quote;
    use std::str::FromStr;
    use syn::ItemImpl;

    fn assert_derive_eq(input: &str, expected: TokenStream) {
        let input = TokenStream::from_str(input).unwrap();
        let expected: ItemImpl = syn::parse2(expected).unwrap();

        let result = impl_repr_c(input, check_attributes).unwrap();
        assert_eq!(expected, syn::parse2(result).unwrap())
    }

    #[test]
    fn derive_ok() {
        let input = "
            #[repr(C)]
            struct Test {
                id: usize,
                name: String,
                points: f64
            }
        ";
        let expected = quote! {
            unsafe impl ::mem::alloc::ReprC for Test {
                type Fields = (usize, String, f64,);
            }
        };

        assert_derive_eq(input, expected)
    }

    #[test]
    fn derive_generics() {
        let input = "
            #[repr(C)]
            struct Test<T: Copy, U>
                where U: Display
            {
                id: usize,
                name: U,
                points: T,
            }
        ";
        let expected = quote! {
            unsafe impl<T: Copy, U> ::mem::alloc::ReprC for Test<T, U>
                where U: Display
            {
                type Fields = (usize, U, T,);
            }
        };

        assert_derive_eq(input, expected)
    }

    #[test]
    fn derive_enum() {
        let input = "
            #[repr(C)]
            enum Question<T> {
                Yes(T), No
            }
        ";

        let input = TokenStream::from_str(input).unwrap();
        let err = impl_repr_c(input, check_attributes).unwrap_err();

        assert_eq!(
            err.to_string(),
            not_a_struct("Question", "enum").to_string()
        )
    }

    #[test]
    fn derive_no_repr() {
        let input = "
            struct Simple {
                simple: i32
            }
        ";

        let input = TokenStream::from_str(input).unwrap();
        let err = impl_repr_c(input, check_attributes).unwrap_err();

        assert_ne!(
            err.to_string(),
            not_a_struct("Simple", "struct").to_string()
        )
    }
}
