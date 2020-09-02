//! Data in memory layout of objects.

mod layout;
mod repr_c;

use std::alloc::LayoutErr;
type Result<T> = std::result::Result<T, LayoutErr>;

pub use layout::Layout;
