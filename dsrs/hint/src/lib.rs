//! Compiler hints and assertions.
#![feature(core_intrinsics)]
#![feature(const_fn)]
#![feature(const_panic)]
#![feature(const_unreachable_unchecked)]
#![feature(const_likely)]
#![warn(clippy::all, clippy::pedantic, clippy::nursery, clippy::cargo)]
#![allow(clippy::cargo_common_metadata)]
#![allow(clippy::multiple_crate_versions)]
#![allow(clippy::wildcard_dependencies)]

pub mod conditions;
pub mod unreachable;
