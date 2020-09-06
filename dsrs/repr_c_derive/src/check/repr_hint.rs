//! Syntax trees for `#[repr(...)]` attribute hints.
use proc_macro2::{TokenStream, Span, Ident, Group};
use syn::parse::{Result, Error, Parse, ParseStream};
use quote::{ToTokens, TokenStreamExt};
use std::convert::TryFrom;

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
    pub ident: Ident,
    pub args: Option<Group>
}

impl ReprHint {
    /// Returns the `Span` for the entire hint, including its identifier and
    /// arguments.
    #[inline]
    pub fn span(&self) -> Span {
        let start = self.ident.span();

        match self.args {
            Some(ref group) => {
                start.join(group.span_close()).unwrap()
            },
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
    pub fn args(&self) -> Option<&Group> {
        self.args.as_ref()
    }
}

impl Parse for ReprHint {
    #[inline]
    fn parse(input: ParseStream<'_>) -> Result<ReprHint> {
        let ident = input.parse()?;
        let args = input.parse()?;
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
    pub span: Span
}

impl ReprCHint {
    /// Just the `C` identifier, at the right [`Span`].
    #[inline]
    fn indent(&self) -> Ident {
        (*self).into()
    }
}

impl Into<ReprHint> for ReprCHint {
    #[inline]
    fn into(self) -> ReprHint {
        ReprHint {
            ident: self.indent(),
            args: None
        }
    }
}

impl Into<Ident> for ReprCHint {
    #[inline]
    fn into(self) -> Ident {
        Ident::new("C", self.span)
    }
}

impl TryFrom<&ReprHint> for ReprCHint {
    type Error = Error;

    #[inline]
    fn try_from(hint: &ReprHint) -> Result<Self> {
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
