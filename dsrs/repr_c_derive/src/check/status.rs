//! Checking presence of `#[repr(C)]` attributes.
use proc_macro2::Span;
use syn::parse::Error;
use std::ops::BitOr;
use super::Result;

/// Status marker for `#[repr(C)]` attribute.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub(super) enum Status {
    /// No `C` hint found yet.
    Missing,
    /// Found at least one `#[repr(C)]` attribute or equivalent.
    Found
}
use Status::{Missing, Found};

impl Status {
    /// Returns [`Found`] when at least one of them is marked so.
    #[inline]
    pub const fn or(self, other: Self) -> Self {
        match (self, other) {
            // or missing when both are as well
            (Missing, Missing) => Missing,
            (_, _) => Found
        }
    }

    /// Turns a [`Missing`] into an [`Error`] at call site.
    ///
    /// `Found`s are `Ok`.
    #[inline]
    pub fn into_result(self) ->  Result {
        match self {
            Found => Ok(()),
            Missing => Err(Self::error())
        }
    }

    /// Default error for when `C` hint is missing.
    /// Generated at [call site](Span::call_site).
    #[inline]
    pub (super) fn error() -> Error {
        let message = "missing '#[repr(C)]' attribute: \
            'ReprC' trait cannot be safely implemented for other layouts";

        Error::new(Span::call_site(), message)
    }
}

impl Default for Status {
    #[inline]
    fn default() -> Self {
        Missing
    }
}

impl BitOr for Status {
    type Output = Self;

    #[inline]
    fn bitor(self, other: Self) -> Self {
        self.or(other)
    }
}

impl Into<Result> for Status {
    #[inline]
    fn into(self) -> Result {
        self.into_result()
    }
}

#[cfg(test)]
mod tests {
    use super::{Missing, Found};

    #[allow(clippy::eq_op)]
    #[test]
    fn bitor() {
        assert_eq!(Missing | Missing, Missing);
        assert_eq!(Missing | Found, Found);
        assert_eq!(Found | Missing, Found);
        assert_eq!(Found | Found, Found);
    }

    #[test]
    fn result() {
        assert!(Missing.into_result().is_err());
        assert!(Found.into_result().is_ok());
    }
}
