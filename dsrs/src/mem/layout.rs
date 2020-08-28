use std::alloc::{Layout as Inner, LayoutErr};


#[inline(always)]
/// Get `(size, align)` for [`Sized`] type
const fn size_align<T>() -> (usize, usize) {
    use std::mem::{size_of, align_of};
    (size_of::<T>(), align_of::<T>())
}

#[inline(always)]
/// Get `(size, align)` from reference
const fn size_align_val<T: ?Sized>(val: &T) -> (usize, usize) {
    use std::mem::{size_of_val, align_of_val};
    (size_of_val(val), align_of_val(val))
}

/// Get `(size, align)` from raw pointer
#[inline(always)]
const unsafe fn size_align_val_raw<T: ?Sized>(ptr: *const T) -> (usize, usize) {
    size_align_val(&*ptr)
}

/// Maximum of two `usize`s
#[inline(always)]
const fn max(a: usize, b: usize) -> usize {
    if a > b {
        a
    } else {
        b
    }
}

/// Wrapper for [`std::alloc::Layout`]
///
/// This wrapper makes most methods `const`.
#[derive(Debug, Copy, Clone, Eq, PartialEq)]
#[repr(transparent)]
pub struct Layout(pub std::alloc::Layout);

impl Layout {
    /// Read [`std::alloc::Layout::from_size_align_unchecked`]
    #[inline]
    pub const unsafe fn from_size_align_unchecked(size: usize, align: usize) -> Self {
        Self(Inner::from_size_align_unchecked(size, align))
    }

    /// Read [`std::alloc::Layout::from_size_align`]
    #[inline]
    pub const fn from_size_align(size: usize, align: usize) -> Result<Self, LayoutErr> {
        match Inner::from_size_align(size, align) {
            Ok(inner) => Ok(Self(inner)),
            Err(err) => Err(err)
        }
    }

    /// Read [`std::alloc::Layout::size`]
    #[inline]
    pub const fn size(&self) -> usize {
        self.0.size()
    }

    /// Read [`std::alloc::Layout::align`]
    #[inline]
    pub const fn align(&self) -> usize {
        self.0.align()
    }

    /// Read [`std::alloc::Layout::new`]
    #[inline]
    pub const fn new<T>() -> Self {
        let (size, align) = size_align::<T>();
        // SAFETY: rust types garantees
        unsafe { Self::from_size_align_unchecked(size, align) }
    }

    /// Read [`std::alloc::Layout::for_value`]
    #[inline]
    pub const fn for_value<T: ?Sized>(val: &T) -> Self {
        let (size, align) = size_align_val(val);
        debug_assert!(Self::from_size_align(size, align).is_ok());
        // SAFETY: rust types garantees
        unsafe { Self::from_size_align_unchecked(size, align) }
    }

    /// Read [`std::alloc::Layout::for_value_raw`]
    #[inline]
    pub const unsafe fn for_value_raw<T: ?Sized>(val: *const T) -> Self {
        let (size, align) = size_align_val_raw(val);
        debug_assert!(Self::from_size_align(size, align).is_ok());
        Self::from_size_align_unchecked(size, align)
    }

    /// Read [`std::alloc::Layout::padding_needed_for`]
    #[inline]
    pub const fn padding_needed_for(&self, align: usize) -> usize {
        let len = self.size();

        let len_rounded_up = len.wrapping_add(align).wrapping_sub(1) & !align.wrapping_sub(1);
        len_rounded_up.wrapping_sub(len)
    }

    /// Read [`std::alloc::Layout::pad_to_align`]
    #[inline]
    pub const fn pad_to_align(&self) -> Self {
        let pad = self.padding_needed_for(self.align());
        // SAFETY: cannot overflow
        let new_size = self.size() + pad;

        unsafe { Self::from_size_align_unchecked(new_size, self.align()) }
    }

    /// Read [`std::alloc::Layout::extend`]
    #[inline]
    pub const fn extend(&self, next: Self) -> Result<(Self, usize), LayoutErr> {
        const LAYOUT_ERR: LayoutErr = match Inner::from_size_align(0, 0) {
            Err(err) => err,
            Ok(_) => unreachable!()
        };
        debug_assert!(std::mem::size_of::<LayoutErr>() == 0);

        let new_align = max(self.align(), next.align());
        let pad = self.padding_needed_for(next.align());

        let offset = match self.size().checked_add(pad) {
            Some(offset) => offset,
            None => return Err(LAYOUT_ERR)
        };
        let new_size = match offset.checked_add(next.size()) {
            Some(size) => size,
            None => return Err(LAYOUT_ERR)
        };

        match Self::from_size_align(new_size, new_align) {
            Err(err) => Err(err),
            Ok(layout) => Ok((layout, offset))
        }
    }

    /// Recover inner [`std::alloc::Layout`] from `Layout`
    #[inline]
    pub const fn inner(self) -> std::alloc::Layout {
        self.0
    }
}


#[cfg(test)]
mod tests {
    use super::*;

    /// Guarantees that `Layout` method equivalence
    #[test]
    fn equivalent_methods() {
        use std::alloc::Layout as Inner;

        type T1 = ();
        type T2 = String;
        let layout1 = Layout::new::<T1>();
        let layout2 = Layout::new::<T2>();

        assert_eq!(layout1.inner(), Inner::new::<T1>());
        assert_eq!(layout2.inner(), Inner::new::<T2>());

        assert_eq!(Layout::from_size_align(10, 4), Inner::from_size_align(10, 4).map(Layout));
        assert_eq!(Layout::from_size_align(13, 7), Inner::from_size_align(13, 7).map(Layout));
        const MAX: usize = std::usize::MAX;
        assert_eq!(Layout::from_size_align(MAX, 16), Inner::from_size_align(MAX, 16).map(Layout));

        // SAFETY: Layout not used
        unsafe { assert_eq!(Layout::from_size_align_unchecked(24, 8), Layout(Inner::from_size_align_unchecked(24, 8))); }

        assert_eq!(layout1.align(), layout1.inner().align());
        assert_eq!(layout2.size(), layout2.inner().size());

        let val = "string";
        assert_eq!(Layout::for_value(val), Layout(Inner::for_value(val)));
        let ptr = val as *const str;
        // SAFETY: `ptr` is a valid reference
        unsafe { assert_eq!(Layout::for_value_raw(ptr), Layout(Inner::for_value_raw(ptr))); }

        assert_eq!(layout2.padding_needed_for(256), layout2.inner().padding_needed_for(256));
        assert_eq!(layout1.pad_to_align().inner(), layout1.inner().pad_to_align());

        assert_eq!(layout1.extend(layout2), layout1.inner().extend(layout2.inner()).map(|(a, b)| (Layout(a), b)));
        let overflow = Layout::from_size_align(MAX - 4, 2).unwrap();
        assert_eq!(layout2.extend(overflow), layout2.inner().extend(overflow.inner()).map(|(a, b)| (Layout(a), b)));
    }
}
