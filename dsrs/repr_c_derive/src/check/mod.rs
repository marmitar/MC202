//! Parsing and checking for `#[repr(...)]` attributes.
mod hints;
mod attr;
mod status;
mod result;

pub use hints::{ReprHint, ReprCHint};
pub use attr::AttrRepr;
use status::Status;


/// Specialized [`Result`](syn::parse::Result) for checking.
///
/// This represents a computation, without result, that may encounter errors.
pub type Result = syn::parse::Result<()>;

/// Special [`and`][and] that [`combine`][err]s errors when both are `Err`.
///
/// [and]: std::result::Result::and
/// [err]: syn::parse::Error::combine
pub fn combine(earlier: Result, later: Result) -> Result {
    match (earlier, later) {
        (Ok(_), result) => result,
        (error, Ok(_)) => error,
        (Err(mut old), Err(new)) => {
            old.combine(new);
            Err(old)
        }
    }
}
