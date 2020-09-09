//! Syntax trees for `#[repr(...)]` attributes.
use proc_macro2::{Delimiter, Group, Ident, Span, TokenStream};
use quote::ToTokens;
use std::convert::TryFrom;
use syn::parse::{Error, Parse, ParseStream, Parser};
use syn::punctuated::{IntoIter, Iter, IterMut, Punctuated};
use syn::token::{Bracket, Paren};
use syn::{AttrStyle, Attribute, Path, PathSegment, Result, Token};

use super::ReprHint;

/// A `repr` attribute like `#[repr(C, aligned(8))]`.
///
/// ```text
/// #[repr(i32, packed(8))]
/// ^^^^^^^^^^^^^^^^^^^^^^^  a 'repr' attribute
/// ```
///
/// All `#[repr(...)]` attributes are [outer attributes], so they can't have a
/// [`Bang`](struct@syn::token::Bang).
///
/// ```text
/// #![feature(proc_macro)]
///                          NOT a 'repr' attribute
/// ```
///
/// The attribute [`path`](Attribute#fields) must be composed of exactly one
/// identifier, which should be equivalent to `"repr"`.
///
/// ```text
/// #[derive(Debug)]
///                          NOT a 'repr' attribute
/// ```
///
/// [outer attributes]: https://doc.rust-lang.org/reference/attributes.html
#[derive(Debug, Clone)]
pub struct AttrRepr {
    pub pound: Token![#],
    pub bracket: Bracket,
    pub ident_span: Span,
    pub paren: Paren,
    pub hints: Punctuated<ReprHint, Token![,]>,
}

impl AttrRepr {
    /// The entire attribute `Span`.
    ///
    /// ```text
    /// #[repr(C, align(4))]
    /// ^^^^^^^^^^^^^^^^^^^^
    /// ```
    #[inline]
    pub fn span(&self) -> Span {
        // start at the pound
        let start = self.pound.spans[0];
        // and end at the last bracket
        let end = self.bracket.span;

        start.join(end).unwrap()
    }

    /// The `repr` identifier, with the right span.
    ///
    /// ```text
    /// #[repr(C, align(4))]
    ///   ^^^^
    /// ```
    #[inline]
    pub fn ident(&self) -> Ident {
        Ident::new("repr", self.ident_span)
    }

    /// The equivalent [`path`][Attribute#fields] for the `repr` identifier.
    ///
    /// This a path with just the `repr` [`ident`](Self::ident).
    #[inline]
    pub fn path(&self) -> Path {
        PathSegment::from(self.ident()).into()
    }

    /// The attribute arguments.
    ///
    /// ```text
    /// #[repr(C, align(4))]
    ///       ^^^^^^^^^^^^^ these are the arguments
    /// ```
    #[inline]
    pub fn arguments(&self) -> Group {
        let span = self.paren.span;
        let hints = self.hints.to_token_stream();
        let paren = Delimiter::Parenthesis;

        let mut group = Group::new(paren, hints);
        group.set_span(span);

        group
    }

    /// Iterate over the [hints](ReprHint) for this attribute.
    #[inline]
    pub fn iter(&self) -> Iter<'_, ReprHint> {
        self.hints.iter()
    }

    /// Mutable version of [`iter`](Self::iter).
    #[inline]
    pub fn iter_mut(&mut self) -> IterMut<'_, ReprHint> {
        self.hints.iter_mut()
    }
}

impl PartialEq for AttrRepr {
    #[inline]
    fn eq(&self, other: &Self) -> bool {
        self.pound == other.pound
            && self.bracket == other.bracket
            && self.ident() == other.ident()
            && self.paren == other.paren
            && self.hints == other.hints
    }
}

impl PartialEq<Attribute> for AttrRepr {
    #[inline]
    fn eq(&self, other: &Attribute) -> bool {
        self.pound == other.pound_token
            && other.style == AttrStyle::Outer
            && self.bracket == other.bracket_token
            && self.path() == other.path
            && syn::parse2::<Group>(other.tokens.clone())
                .ok()
                .filter(|group| {
                    group.delimiter() == Delimiter::Parenthesis
                        && Parser::parse2(Punctuated::parse_terminated, group.stream())
                            .ok()
                            .filter(|parsed| parsed == &self.hints)
                            .is_some()
                })
                .is_some()
    }
}

impl IntoIterator for AttrRepr {
    type Item = ReprHint;
    type IntoIter = IntoIter<ReprHint>;

    #[inline]
    fn into_iter(self) -> IntoIter<ReprHint> {
        self.hints.into_iter()
    }
}

impl<'a> IntoIterator for &'a AttrRepr {
    type Item = &'a ReprHint;
    type IntoIter = Iter<'a, ReprHint>;

    #[inline]
    fn into_iter(self) -> Iter<'a, ReprHint> {
        self.iter()
    }
}

impl<'a> IntoIterator for &'a mut AttrRepr {
    type Item = &'a mut ReprHint;
    type IntoIter = IterMut<'a, ReprHint>;

    #[inline]
    fn into_iter(self) -> IterMut<'a, ReprHint> {
        self.iter_mut()
    }
}

impl ToTokens for AttrRepr {
    #[inline]
    fn to_tokens(&self, tokens: &mut TokenStream) {
        self.pound.to_tokens(tokens);

        self.bracket.surround(tokens, |tokens| {
            self.path().to_tokens(tokens);

            self.paren.surround(tokens, |tokens| {
                self.hints.to_tokens(tokens);
            })
        });
    }

    #[inline]
    fn into_token_stream(self) -> TokenStream {
        Into::<Attribute>::into(self).into_token_stream()
    }
}

impl Into<Attribute> for AttrRepr {
    #[inline]
    fn into(self) -> Attribute {
        Attribute {
            pound_token: self.pound,
            style: AttrStyle::Outer,
            bracket_token: self.bracket,
            path: self.path(),
            tokens: self.arguments().into_token_stream(),
        }
    }
}

/// Get the `Span` of a `"repr"` identifier.
///
/// Returns `Err` when the ident is different from `repr`.
#[inline]
fn repr_ident_span(ident: &Ident) -> Result<Span> {
    if ident == "repr" {
        Ok(ident.span())
    } else {
        let message = format!("wrong identifier '{}', expected 'repr'", ident);
        Err(Error::new(ident.span(), message))
    }
}

impl TryFrom<Attribute> for AttrRepr {
    type Error = Error;

    #[inline]
    fn try_from(attr: Attribute) -> Result<Self> {
        // the pound token
        let pound = attr.pound_token;
        // check if outer attribute
        if let AttrStyle::Inner(bang) = attr.style {
            let message = "'repr' must be an outer attribute";
            return Err(Error::new(bang.span, message))
        }
        // bracket tokens
        let bracket = attr.bracket_token;

        // the 'repr' identifier
        let ident_span = attr
            .path
            .get_ident()
            .ok_or_else(|| {
                let message = "wrong path, 'repr' is an intrinsic Rust attribute";
                Error::new_spanned(&attr.path, message)
            })
            .and_then(repr_ident_span)?;

        // the arguments group
        let group: Group = syn::parse2(attr.tokens)?;
        // which must be a parenthesis
        if group.delimiter() != Delimiter::Parenthesis {
            return Err(Error::new(group.span_open(), "expected '('"))
        }
        let paren = Paren { span: group.span() };

        // the hint list must have no leading or trailing comma
        let hints = Parser::parse2(Punctuated::parse_terminated, group.stream())?;

        Ok(Self {
            pound,
            bracket,
            ident_span,
            paren,
            hints,
        })
    }
}

impl Parse for AttrRepr {
    #[inline]
    fn parse(input: ParseStream<'_>) -> Result<Self> {
        // sub tokens streams
        let (attribute, arguments);

        let pound = input.parse()?;
        let bracket = syn::bracketed!(attribute in input);
        let ident_span = Ident::parse(&attribute).and_then(|ref id| repr_ident_span(id))?;
        let paren = syn::parenthesized!(arguments in attribute);
        let hints = Punctuated::parse_terminated(&arguments)?;

        Ok(Self {
            pound,
            bracket,
            ident_span,
            paren,
            hints,
        })
    }
}

#[cfg(test)]
mod tests {
    use super::AttrRepr;

    use std::convert::TryInto;
    use syn::Attribute;

    #[test]
    fn parsing() {
        let parse = syn::parse_str::<AttrRepr>;

        assert!(parse("#[repr(C)]").is_ok());
        assert!(parse("#[repr(C, C,C)]").is_ok());
        assert!(parse("#[repr(align(8))]").is_ok());
        assert!(parse("#[repr(transparent, align(1))]").is_ok());
        assert!(parse("#[repr(rust, packed(u12))]").is_ok());
        assert!(parse("#[repr(C,C,)]").is_ok());
        assert!(parse("#[repr()]").is_ok());

        assert!(parse("#[repr]").is_err());
        assert!(parse("#[repr(,)]").is_err());
        assert!(parse("#[attr]").is_err());
        assert!(parse("#![repr]").is_err());
    }

    #[test]
    fn revertible() {
        let parse = syn::parse_str::<AttrRepr>;

        let attr_a = parse("#[repr(C)]").unwrap();
        let attr: Attribute = attr_a.clone().into();
        assert_eq!(attr_a, attr);
        let attr: AttrRepr = attr.try_into().unwrap();
        assert_eq!(attr_a, attr);

        let attr_a = parse("#[repr(C, C,C)]").unwrap();
        let attr: Attribute = attr_a.clone().into();
        assert_eq!(attr_a, attr);
        let attr: AttrRepr = attr.try_into().unwrap();
        assert_eq!(attr_a, attr);

        let attr_a = parse("#[repr(transparent, align(1))]").unwrap();
        let attr: Attribute = attr_a.clone().into();
        assert_eq!(attr_a, attr);
        let attr: AttrRepr = attr.try_into().unwrap();
        assert_eq!(attr_a, attr);

        let err = parse("#[derive(ReprC)]").unwrap_err();
        assert!(err.to_string().starts_with("wrong identifier 'derive'"));
    }
}
