use proc_macro2::{Ident, Span, TokenStream};
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
        impl #impl_generics ::mem::alloc::ReprC for #name #ty_generics #where_clause {
            type Fields = (#(#field,)*);
        }
    })
}

/// Generate standard error for when given type is not a struct.
///
/// `ident` should be the struct name and `data_type` is a
/// string describing the kind of the type (`enum` or `union`).
fn not_a_struct(ident: &Ident, data_type: &str) -> Error {
    let message = format!(
        "wrong type layout: 'ReprC' can only be \
        implemented for structs, but {} is an {}",
        ident, data_type
    );

    Error::new(Span::call_site(), message)
}
