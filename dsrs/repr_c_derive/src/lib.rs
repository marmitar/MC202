//! Derive macros for [`ReprC`](mem::alloc::ReprC) trait.
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

/// Derive macro for [`ReprC`](mem::alloc::ReprC) trait.
///
/// As explained in the documentation for the trait, `ReprC` can only be
/// safely implemented for `struct`s marked as `#[repr(C)]`. This macro will
/// fail to compile if any of these constraints are not held.
///
/// # Examples
///
/// ```
/// # use repr_c_derive::ReprC;
/// #[derive(ReprC)]
/// #[repr(C)]
/// struct SomeData<T> {
///     name: String,
///     data: T
/// }
/// ```
///
/// Which is the same as:
///
/// ```
/// use mem::alloc::ReprC;
///
/// #[repr(C)]
/// struct SomeData<T> {
///     name: String,
///     data: T
/// }
///
/// unsafe impl<T> ReprC for SomeData<T> {
///     type Fields = (String, T);
/// }
/// ```
///
/// # Compile errors
///
/// Note however that the macro will deny implementation for
/// non-`#[repr(C)]` structs.
///
/// ```compile_fail
/// # use repr_c_derive::ReprC;
/// #[derive(ReprC)]
/// struct SomeData<T> {
///     name: String,
///     data: T
/// }
/// ```
///
/// Or if there are other `#[repr(...)]` hints for the struct.
///
/// ```compile_fail
/// # use repr_c_derive::ReprC;
/// #[derive(ReprC)]
/// #[repr(C, align(8))]
/// struct Wrapper {
///     data: u64
/// }
/// ```
///
/// And the trait can only be implemented on `struct`s and will
/// cause errors for `enum`s or `union`s.
///
/// ```compile_fail
/// # use repr_c_derive::ReprC;
/// #[derive(ReprC)]
/// #[repr(C)]
/// enum MaybeChar {
///     Some(char),
///     Nothing
/// }
/// ```
#[proc_macro_derive(ReprC)]
pub fn repr_c_derive(input: TokenStream) -> TokenStream {

    match impl_repr_c(input.into(), check_attributes) {
        Ok(derive) => derive,
        Err(err) => err.to_compile_error(),
    }
    .into()
}

/// Unsafe version of [`ReprC`].
///
/// This macro implements the [`ReprC`](mem::alloc::ReprC) trait without
/// checking for `#[repr(C)]` attributes and ignoring any invalid layout
/// hint.
///
/// # Safety
///
/// This is only safe if the user guarantees that that the struct will
/// follow a `#[repr(C)]` layout.
///
/// # Compile errors
///
/// This macro will still cause errors if the type is not a `struct`.
/// As implementation can be *extremely* unsafe in that case.
#[proc_macro_derive(Unsafe_ReprC)]
pub fn repr_c_unsafe_derive(input: TokenStream) -> TokenStream {
    let skip_check = |_| Ok(());

    match impl_repr_c(input.into(), skip_check) {
        Ok(derive) => derive,
        Err(err) => err.to_compile_error(),
    }
    .into()
}
