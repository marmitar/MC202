//! Data in memory layout.

use std::alloc::{Layout as Inner, LayoutErr};
use std::mem::{size_of, align_of};


/// Get `(size, align)` for [`Sized`] types.
#[inline(always)]
const fn size_align<T>() -> (usize, usize) {
    (size_of::<T>(), align_of::<T>())
}

/// Get `(size, align)` from reference.
#[inline(always)]
const fn size_align_val<T: ?Sized>(val: &T) -> (usize, usize) {
    use std::mem::{size_of_val, align_of_val};
    (size_of_val(val), align_of_val(val))
}

/// Get `(size, align)` from raw pointer.
#[inline(always)]
const unsafe fn size_align_val_raw<T: ?Sized>(ptr: *const T) -> (usize, usize) {
    size_align_val(&*ptr)
}

/// Maximum of two `usize`s.
#[inline(always)]
const fn max(a: usize, b: usize) -> usize {
    if hint::likely!(a >= b) {
        a
    } else {
        b
    }
}

/// Instance of [`LayoutErr`].
const LAYOUT_ERR: LayoutErr = match Inner::from_size_align(0, 0) {
    // guarantee that the error is a unit type
    Err(err) if size_of::<LayoutErr>() == 0 => err,
    _ => unreachable!()
};

/// Wrapper for [`std::alloc::Layout`].
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
        // 0 is not a power of 2
        if hint::unlikely!(!align.is_power_of_two()) {
            return Err(LAYOUT_ERR);
        }
        // size_rounded_up = (size + align - 1) & !(align - 1);
        if hint::unlikely!(size > usize::MAX - (align - 1)) {
            return Err(LAYOUT_ERR);
        }

        // SAFETY: checks above
        Ok(unsafe { Self::from_size_align_unchecked(size, align) })
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
        // SAFETY: `Layout` type guarantees
        let len_rounded_up = len.wrapping_add(align).wrapping_sub(1) & !align.wrapping_sub(1);

        len_rounded_up.wrapping_sub(len)
    }

    /// Read [`std::alloc::Layout::pad_to_align`]
    #[inline]
    pub const fn pad_to_align(&self) -> Self {
        let pad = self.padding_needed_for(self.align());
        // SAFETY: cannot overflow
        let new_size = self.size() + pad;

        // SAFETY: guaranteed by `padding_needed_for`
        unsafe { Self::from_size_align_unchecked(new_size, self.align()) }
    }

    /// Read [`std::alloc::Layout::extend`]
    #[inline]
    pub const fn extend(&self, next: Self) -> Result<(Self, usize), LayoutErr> {
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

        // SAFETY: the old Layouts already checked for power of 2
        unsafe { hint::assume!(new_align.is_power_of_two()); }
        match Self::from_size_align(new_size, new_align) {
            Err(err) => Err(err),
            Ok(layout) => Ok((layout, offset))
        }
    }

    /// Const version of [`PartialEq::eq`].
    #[inline]
    pub const fn eq(&self, other: &Self) -> bool {
        self.size() == other.size() && self.align() == other.align()
    }

    /// Recover inner [`std::alloc::Layout`] from `Layout`.
    #[inline(always)]
    pub const fn inner(self) -> std::alloc::Layout {
        self.0
    }
}
