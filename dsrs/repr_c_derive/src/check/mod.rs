//! Parsing and checking for `#[repr(...)]` attributes.
#![allow(clippy::module_name_repetitions)]
mod attr;
mod hints;
mod result;
mod status;

use attr::AttrRepr;
use hints::{ReprCHint, ReprHint};
use result::ReprResult;
use status::Status;

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
        },
    }
}

/// Checks a list of attributes for any offending layout hint.
///
/// # Errors
///
/// This will accuse any `#[repr(...)]` attribute wich is not equivalent to
/// `#[repr(C)]`. No other layouts can safely implement the
/// [`ReprC`](../mem/alloc/trait.ReprC.html) trait.
///
/// Also, if `#[repr(C)]` is not used at all, this will create another error
/// at call site.
#[inline]
pub fn check_attributes<I: IntoIterator<Item = Attribute>>(iter: I) -> Result {
    iter.into_iter().collect::<ReprResult>().into()
}

#[cfg(test)]
mod tests {
    use super::{check_attributes, combine};
    use super::{ReprResult, Result, Status};

    use proc_macro2::Span;
    use syn::parse::{Error, Parser};
    use syn::Attribute;

    #[track_caller]
    fn assert_result_eq(a: &Result, b: &Result) {
        assert_eq!(
            a.as_ref().map_err(|e| format!("{:?}", e)),
            b.as_ref().map_err(|e| format!("{:?}", e))
        )
    }

    #[track_caller]
    fn assert_result_ne(a: &Result, b: &Result) {
        assert_ne!(
            a.as_ref().map_err(|e| format!("{:?}", e)),
            b.as_ref().map_err(|e| format!("{:?}", e))
        )
    }

    #[test]
    fn combining_errors() {
        let text = "#[attr1]\n#[attr2]";
        let attr = Parser::parse_str(Attribute::parse_outer, text).unwrap();

        let err1 = Err(Error::new_spanned(&attr[0], "original error"));
        let err2 = Err(Error::new_spanned(&attr[1], "error that came after"));

        let comb = combine(err1.clone(), err2.clone());

        assert_result_ne(&err1, &err2);
        assert_result_ne(&err1, &comb);
        assert_result_ne(&err2, &comb);

        assert_result_eq(&combine(Ok(()), err1.clone()), &err1);
        assert_result_eq(&combine(Ok(()), err2.clone()), &err2);
        assert_result_eq(&combine(comb.clone(), Ok(())), &comb);

        assert_result_ne(&combine(err2.clone(), err1.clone()), &comb);
        assert_result_eq(&combine(err1.clone(), err2.clone()), &comb);
        assert_result_eq(
            &combine(combine(err1, Ok(())), combine(Ok(()), err2)),
            &comb,
        );
    }

    #[test]
    fn attribute_checking() {
        let text_ok = "
        #[repr(C)]
        #[derive(Debug)]
        #[another_attribute]
        ";
        let text_err = "
        #[repr(C, aligned(1))]
        #[derive(Clone)]
        ";
        let text_miss = "
        #[rep(C)]
        #[derive(Clone, Debug)]
        ";

        let attrs = Parser::parse_str(Attribute::parse_outer, text_ok).unwrap();
        assert_result_eq(&check_attributes(attrs), &Ok(()));

        let attrs = Parser::parse_str(Attribute::parse_outer, text_err).unwrap();
        let err = Err(ReprResult::error(Span::call_site()));
        assert_result_eq(&check_attributes(attrs), &err);

        let attrs = Parser::parse_str(Attribute::parse_outer, text_miss).unwrap();
        let err = Err(Status::error());
        assert_result_eq(&check_attributes(attrs), &err);
    }
}
