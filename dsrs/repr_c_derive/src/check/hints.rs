//! Syntax trees for `#[repr(...)]` attribute hints.
use proc_macro2::Delimiter::Parenthesis;
use proc_macro2::{Group, Ident, Span, TokenStream};
use quote::{ToTokens, TokenStreamExt};
use std::convert::TryFrom;
use syn::parse::{Error, Parse, ParseStream, Result};

/// Hints for the `#[repr(...)]` attribute.
///
/// These are the arguments that go inside the attribute.
///
/// ```text
/// #[repr(transparent)]
///        ^^^^^^^^^^^ this is a hint
/// ```
///
/// In Rust syntax, they can be comma separeted, with no leading or trailing
/// comma.
///
/// ```text
/// #[repr(C, align(8))]
///        ^  ^^^^^^^^ two hints here
/// ```
#[derive(Debug, Clone)]
pub struct ReprHint {
    ident: Ident,
    args: Option<Group>,
}

impl ReprHint {
    /// Returns the `Span` for the entire hint, including its identifier and
    /// arguments.
    #[inline]
    pub fn span(&self) -> Span {
        let start = self.ident.span();

        match self.args {
            Some(ref group) => start.join(group.span_close()).unwrap(),
            None => start,
        }
    }

    /// The hint identifier.
    ///
    /// Most common ones are `C`, `transparent`, `align`, `packed` and the
    /// [integer representation] for fieldless `enum`s.
    ///
    /// [integer representation]: https://doc.rust-lang.org/nomicon/other-reprs.html#repru-repri
    #[inline]
    pub const fn ident(&self) -> &Ident {
        &self.ident
    }

    /// The arguments for the hint.
    ///
    /// Most hints can't take arguments, but `align`, as an example, can.
    /// The arguments are always parenthesized.
    ///
    /// ```text
    /// #[repr(align(8))]
    ///             ^^^ these are the arguments
    /// ```
    #[inline]
    pub const fn args(&self) -> Option<&Group> {
        self.args.as_ref()
    }
}

impl Parse for ReprHint {
    #[inline]
    fn parse(input: ParseStream<'_>) -> Result<Self> {
        let ident = input.parse()?;
        let args = match Option::<Group>::parse(input)? {
            Some(group) if group.delimiter() != Parenthesis => {
                let message = "unexpected delimiter, \
                    repr hints can only have parenthesis";
                return Err(Error::new(group.span_open(), message))
            },
            args => args,
        };

        Ok(Self { ident, args })
    }
}

impl ToTokens for ReprHint {
    #[inline]
    fn to_tokens(&self, tokens: &mut TokenStream) {
        tokens.append(self.ident.clone());
        if let Some(ref group) = self.args {
            tokens.append(group.clone())
        }
    }
}

impl PartialEq for ReprHint {
    #[inline]
    fn eq(&self, other: &Self) -> bool {
        match (self.args(), other.args()) {
            (None, None) => self.ident() == other.ident(),
            (Some(self_args), Some(other_args)) => {
                self.ident() == other.ident() && self_args.to_string() == other_args.to_string()
            },
            _ => false,
        }
    }
}

/// Specialized [`ReprHint`] for the `#[repr(C)]` attribute.
///
/// This is only the `C` identifier, without any arguments.
///
/// ```text
/// #[repr(C, C)]
///        ^  ^  both are valid 'ReprCHint'
/// ```
#[derive(Debug, Copy, Clone)]
pub struct ReprCHint {
    pub span: Span,
}

impl ReprCHint {
    /// Just the `C` identifier, at the right [`Span`].
    #[inline]
    fn ident(self) -> Ident {
        self.into()
    }
}

impl Into<ReprHint> for ReprCHint {
    #[inline]
    fn into(self) -> ReprHint {
        ReprHint {
            ident: self.ident(),
            args: None,
        }
    }
}

impl Into<Ident> for ReprCHint {
    #[inline]
    fn into(self) -> Ident {
        Ident::new("C", self.span)
    }
}

impl TryFrom<ReprHint> for ReprCHint {
    type Error = Error;

    #[inline]
    fn try_from(hint: ReprHint) -> Result<Self> {
        // must have a 'C' identifier
        let (span, msg) = if hint.ident() != "C" {
            let message = "expected 'C' here, for a C layout hint";
            (hint.ident().span(), message)
        // cannot have arguments
        } else if let Some(group) = hint.args() {
            let message = "unexpected arguments to a 'C' repr hint";
            (group.span(), message)
        // only then can it become a 'C' repr hint
        } else {
            return Ok(Self { span: hint.span() })
        };

        Err(Error::new(span, msg))
    }
}

impl Parse for ReprCHint {
    #[inline]
    fn parse(input: ParseStream<'_>) -> Result<Self> {
        use std::convert::TryInto;

        ReprHint::parse(input)?.try_into()
    }
}

impl ToTokens for ReprCHint {
    #[inline]
    fn to_tokens(&self, tokens: &mut TokenStream) {
        self.ident().to_tokens(tokens)
    }
    #[inline]
    fn into_token_stream(self) -> TokenStream {
        self.ident().into_token_stream()
    }
}

impl PartialEq for ReprCHint {
    #[inline]
    fn eq(&self, _other: &Self) -> bool {
        true
    }
}

impl PartialEq<ReprHint> for ReprCHint {
    #[inline]
    fn eq(&self, other: &ReprHint) -> bool {
        other.args().is_none() && &self.ident() == other.ident()
    }
}

#[cfg(test)]
mod tests {
    use super::{ReprCHint, ReprHint};

    use quote::ToTokens;
    use std::convert::TryFrom;

    #[test]
    fn parsing() {
        let parse_hint = syn::parse_str::<ReprHint>;
        let parse_c_hint = syn::parse_str::<ReprCHint>;

        assert!(parse_hint("C").is_ok());
        assert!(parse_hint("aligned(8)").is_ok());
        assert!(parse_hint("transparent").is_ok());
        assert!(parse_hint("packed()").is_ok());
        assert!(parse_hint("C[u8]").is_err());
        assert!(parse_hint("").is_err());

        assert!(parse_c_hint("C").is_ok());
        assert!(parse_c_hint("transparent").is_err());
        assert!(parse_c_hint("A").is_err());
        assert!(parse_c_hint("").is_err());
        assert!(parse_c_hint("C(i8)").is_err());
    }

    #[test]
    fn revertible() {
        let c_hint: ReprCHint = syn::parse_str("C").unwrap();
        assert_eq!("C", c_hint.to_token_stream().to_string());

        let hint_a: ReprHint = syn::parse_str("C").unwrap();
        assert_eq!("C", hint_a.to_token_stream().to_string());
        assert_eq!(hint_a.ident().clone(), c_hint.ident());
        assert!(hint_a.args().is_none());
        assert!(ReprCHint::try_from(hint_a).is_ok());

        let hint_b: ReprHint = syn::parse_str("C(1, 2, 3)").unwrap();
        assert_ne!("C", hint_b.to_token_stream().to_string());
        assert_eq!(hint_b.ident().clone(), c_hint.ident());
        assert!(hint_b.args().is_some());
        assert!(ReprCHint::try_from(hint_b).is_err());

        let hint_c: ReprHint = syn::parse_str("transparent").unwrap();
        assert_ne!("C", hint_c.to_token_stream().to_string());
        assert_ne!(hint_c.ident().clone(), c_hint.ident());
        assert!(hint_c.args().is_none());
        assert!(ReprCHint::try_from(hint_c).is_err());
    }
}
