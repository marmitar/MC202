//! Library with memory management APIs.
//!
//! # `#[repr(C)]` struct management
//!
//! The only non trivial structure representation in Rust with specified
//! layout is based on the C structure layout. So for this kind of struct,
//! it is possible to build the object at the heap, instead of the common way
//! of building the object on the stack and sending it to the heap. Building
//! on the heap is extremely useful when the struct can't be know at compile
//! time, that is, when the implementator struct can't know to be
//! [`Sized`](Sized).
//!
//! For `#[repr(C)]` structs, the trait [`ReprC`](alloc::ReprC) can be safely
//! implemented, when the [`Fields`](alloc::ReprC::Fields) are described
//! correctly. The trait then is capable of safely building a `Box<Self>`
//! with the starting, sized fields [`Start<Self>`](alloc::Start) and a
//! [`Box<Last<Self>>`](alloc::Last) with the last field. The trait can
//! also break apart the structure still on the heap, returning each of
//! its fields.
//!
//! # Wrappers
//!
//! This lib contains wrappers for [`std`], with a bit more `const`ness.
//!
//! * [`NonNull`](ptr::NonNull)
//! * [`Layout`](alloc::Layout)
//!
//! # Utility funtions
//!
//! * [`is_fat_pointer`](ptr::is_fat_pointer): check that `*mut T` is a fat
//!   pointer.
//! * [`update_data`](ptr::update_data): updates fat pointers, keeping the
//!   metadata.
//! * [`update_metadata`](ptr::update_metadata): updates the metadata for fat
//!   pointers.
//! * [`grow`](alloc::grow): increases allocated memory, keeping pointer
//!   metadata.
//! * [`shrink`](alloc::shrink): reduces allocated memory, keeping pointer
//!   metadata.
#![feature(unsafe_block_in_unsafe_fn)]
#![feature(core_intrinsics)]
#![feature(layout_for_ptr)]
#![feature(alloc_layout_extra)]
#![feature(const_fn)]
#![feature(const_unreachable_unchecked)]
#![feature(const_ptr_is_null)]
#![feature(const_ptr_offset)]
#![feature(const_mut_refs)]
#![feature(const_raw_ptr_deref)]
#![feature(const_raw_ptr_to_usize_cast)]
#![feature(const_size_of_val)]
#![feature(const_align_of_val)]
#![feature(const_alloc_layout)]
#![feature(const_result)]
#![feature(const_panic)]
#![feature(const_likely)]
#![feature(unsize)]
#![feature(coerce_unsized)]
#![feature(never_type)]
#![feature(stmt_expr_attributes)]
#![feature(dispatch_from_dyn)]
#![feature(allocator_api)]
#![deny(unsafe_op_in_unsafe_fn)]
// waiting on https://github.com/rust-lang/rust/issues/75913
#![allow(incomplete_features)]
#![feature(const_generics)]
#![warn(clippy::all, clippy::pedantic, clippy::nursery, clippy::cargo)]
#![allow(clippy::cargo_common_metadata)]
#![allow(clippy::multiple_crate_versions)]
#![allow(clippy::wildcard_dependencies)]

pub mod alloc;
pub mod ptr;
