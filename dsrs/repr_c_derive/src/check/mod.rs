//! Parsing and checking for `#[repr(...)]` attributes.
mod hints;
mod attr;
mod status;
mod result;

use hints::{ReprHint, ReprCHint};
use attr::AttrRepr;
use status::Status;
use result::ReprResult;

use syn::Attribute;


/// Specialized [`Result`](syn::parse::Result) for checking.
///
/// This represents a computation, without result, that may encounter errors.
pub type Result = syn::parse::Result<()>;

/// Special [`and`][and] that [`combine`][err]s errors when both are `Err`.
///
/// [and]: std::result::Result::and
/// [err]: syn::parse::Error::combine
#[inline]
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

/// Checks a list of attributes for any offending layout hint.
///
/// # Errors
///
/// This will accuse any `#[repr(...)]` attribute wich is not equivalent to
/// `#[repr(C)]`. No other layouts can safely implement the
/// [`ReprC`](mem::alloc::ReprC) trait.
///
/// Also, if `#[repr(C)]` is not used at all, this will create another error
/// at call site.
#[inline]
pub fn check_attributes<I: IntoIterator<Item=Attribute>>(iter: I) -> Result {
    iter.into_iter()
        .collect::<ReprResult>()
        .into()
}
