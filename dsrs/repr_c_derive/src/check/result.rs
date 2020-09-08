//! Checking validity of `#[repr(...)]` attributes.
use proc_macro2::Span;
use std::convert::{TryFrom, TryInto};
use std::iter::FromIterator;
use syn::parse::Error;

use super::{combine, Result, Status};
use super::{AttrRepr, ReprCHint, ReprHint};
use Status::{Found, Missing};

/// Result tracker for checking `#[repr(...)]` attributes.
#[derive(Debug, Clone)]
pub struct ReprResult {
    /// Current status for `#[repr(C)]` attribute or equivalent.
    repr_c: Status,
    /// Current result for other `#[repr(...)]` attributes.
    others: Result,
}

impl ReprResult {
    /// Combines two results.
    ///
    /// Keeps track of errors from both results and if any of them found
    /// a valid `#[repr(C)]` attribute.
    #[inline]
    pub fn combine(self, other: Self) -> Self {
        Self {
            repr_c: self.repr_c | other.repr_c,
            others: combine(self.others, other.others),
        }
    }

    /// Default result, considering no attributes checked yet.
    #[inline]
    pub(super) const fn missing() -> Self {
        Self {
            repr_c: Missing,
            others: Ok(()),
        }
    }

    /// Result for when a attribute only marks as `#[repr(C)]`.
    #[inline]
    pub(super) const fn found() -> Self {
        Self {
            repr_c: Found,
            others: Ok(()),
        }
    }

    /// Result when another `#[repr(...)]` attribute is found at given `Span`.
    #[inline]
    pub(super) fn other(at: Span) -> Self {
        Self {
            repr_c: Missing,
            others: Err(Self::error(at)),
        }
    }

    /// Default error for when another `repr` hint is found at given `Span`.
    #[inline]
    pub(super) fn error(at: Span) -> Error {
        let message = "only '#[repr(C)]' structs can implement 'ReprC' \
            trait safely, with no other layout hints\
            \n\n\
            Read the documentation for the trait for a better explanation.";

        Error::new(at, message)
    }
}

impl Into<Result> for ReprResult {
    #[inline]
    fn into(self) -> Result {
        combine(self.repr_c.into(), self.others)
    }
}

impl<T: Into<ReprResult>> FromIterator<T> for ReprResult {
    #[inline]
    fn from_iter<I: IntoIterator<Item = T>>(iter: I) -> Self {
        iter.into_iter()
            .map(Into::into)
            .fold(Self::missing(), Self::combine)
    }
}

impl From<ReprCHint> for ReprResult {
    #[inline]
    fn from(_: ReprCHint) -> Self {
        Self::found()
    }
}

impl From<ReprHint> for ReprResult {
    #[inline]
    fn from(hint: ReprHint) -> Self {
        let err_loc = hint.span();

        match ReprCHint::try_from(hint) {
            Ok(c_hint) => Self::from(c_hint),
            Err(_) => Self::other(err_loc),
        }
    }
}

impl Default for ReprResult {
    #[inline]
    fn default() -> Self {
        Self::missing()
    }
}

impl<T: TryInto<AttrRepr>> From<T> for ReprResult {
    #[inline]
    fn from(attr: T) -> Self {
        attr.try_into()
            .map(|attr| attr.into_iter().map(Self::from).collect())
            .unwrap_or_default()
    }
}

impl PartialEq for ReprResult {
    fn eq(&self, other: &Self) -> bool {
        self.repr_c == other.repr_c
            && match (&self.others, &other.others) {
                (Ok(_), Ok(_)) => true,
                (Err(e1), Err(e2)) => e1.to_string() == e2.to_string(),
                (_, _) => false,
            }
    }
}

#[cfg(test)]
mod tests {
    use super::ReprResult as Result;

    use super::AttrRepr;
    use super::ReprCHint as CHint;
    use super::ReprHint as Hint;
    use syn::Attribute;

    use syn::parse::Parser;

    #[test]
    fn basic_cmp() {
        assert_eq!(Result::missing(), Result::missing());
        assert_eq!(Result::found(), Result::found());
        assert_ne!(Result::missing(), Result::found());
    }

    #[test]
    fn hint_parsing() {
        let c_hint: CHint = syn::parse_str("C").unwrap();
        assert_eq!(Result::found(), c_hint.into());
        let hint: Hint = c_hint.into();
        assert_eq!(Result::found(), hint.into());
        let hint: Hint = syn::parse_str("not(C)").unwrap();
        assert_eq!(Result::other(hint.span()), hint.into());
    }

    #[test]
    fn attr_parsing() {
        let parse_repr = |s| syn::parse_str::<AttrRepr>(s).unwrap().into();
        let parse_outer = |s| {
            Parser::parse_str(Attribute::parse_outer, s).unwrap()[0]
                .clone()
                .into()
        };
        let parse_inner = |s| {
            Parser::parse_str(Attribute::parse_inner, s).unwrap()[0]
                .clone()
                .into()
        };

        let result: Vec<Result> = vec![
            parse_repr("#[repr(C)]"),
            parse_outer("#[repr(C, C, C)]"),
            parse_repr("#[repr(C)]"),
            parse_inner("#![repr(packed)]"),
            parse_outer("#[attr]"),
            parse_repr("#[repr(C, packed)]"),
        ];

        assert_eq!(Result::found(), result[0]);
        assert_eq!(Result::found(), result[1]);
        assert_eq!(Result::found(), result[2]);
        assert_eq!(Result::missing(), result[3]);
        assert_eq!(Result::missing(), result[4]);
        assert!(result[5].others.is_err());

        assert_eq!(
            Result::found(),
            result[..3].iter().map(Clone::clone).collect()
        );
        assert_eq!(
            Result::found(),
            result[..5].iter().map(Clone::clone).collect()
        );
        assert_eq!(
            Result::missing(),
            result[3..5].iter().map(Clone::clone).collect()
        );
        assert_eq!(result[5], result[3..6].iter().map(Clone::clone).collect());
        assert_eq!(result[5], result[3..6].iter().map(Clone::clone).collect());
    }
}
