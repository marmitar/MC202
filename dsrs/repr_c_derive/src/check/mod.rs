//! Parsing and checking for `#[repr(..)]` attributes.
mod hint;
mod attr;

use self::hint::{ReprHint, ReprCHint};
use attr::AttrRepr;
