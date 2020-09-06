//! Parsing and checking for `#[repr(...)]` attributes.
mod hints;
mod attr;
mod result;

pub use hints::{ReprHint, ReprCHint};
pub use attr::AttrRepr;
