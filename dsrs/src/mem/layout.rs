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

    /// Const version of [`PartialEq::eq`].
    #[inline]
    pub const fn eq(&self, other: &Self) -> bool {
        self.size() == other.size() && self.align() == other.align()
    }

    /// Layout for an empty, zero-sized `#[repr(C)]` struct.
    ///
    /// ```
    /// # use dsrs::mem::Layout;
    /// #[repr(C)]
    /// struct Empty {}
    ///
    /// assert_eq!(Layout::EMPTY, Layout::new::<Empty>());
    /// ```
    pub const EMPTY: Layout = match Layout::from_size_align(0, 1) {
        Ok(layout) if layout.eq(&layout.pad_to_align()) => layout,
        _ => unreachable!()
    };

    /// Repeatedly apply [`Layout::extend`].
    #[inline]
    pub const fn extend_many<const N: usize>(&self, layouts: [Layout; N]) -> Result<(Layout, [usize; N]), LayoutErr> {
        let mut offsets = [0; N];
        let mut layout = *self;

        let mut i = 0;
        while i < N {
            let (new, offset) = match layout.extend(layouts[i]) {
                Ok(data) => data,
                Err(err) => return Err(err)
            };

            offsets[i] = offset;
            layout = new;

            i += 1;
        }

        Ok((layout, offsets))
    }

    /// Calculate the layout for a `#[repr(C)]` struct and the offset
    /// of its fields, based on the layout of each field. Returns
    /// error on arithmetic overflow. See [`extend`](std::alloc::Layout::extend).
    ///
    /// For a more ergonomic implementation, see [`layout_repr_c`] macro.
    ///
    /// # Example
    ///
    /// ```
    /// use dsrs::mem::Layout;
    ///
    /// #[repr(C)]
    /// struct Struct {
    ///     num: i32,
    ///     text: String
    /// }
    ///
    /// let fields = [Layout::new::<i32>(), Layout::new::<String>()];
    /// let (layout, [num_offset, text_offset]) = Layout::for_repr_c(fields).unwrap();
    ///
    /// assert_eq!(layout, Layout::new::<Struct>());
    /// assert_eq!(num_offset, 0);
    /// ```
    #[inline]
    pub const fn for_repr_c<const N: usize>(fields: [Layout; N]) -> Result<(Layout, [usize; N]), LayoutErr> {
        match Self::EMPTY.extend_many(fields) {
            Ok((layout, offsets)) => Ok((layout.pad_to_align(), offsets)),
            err => err
        }
    }

    /// Recover inner [`std::alloc::Layout`] from `Layout`.
    #[inline(always)]
    pub const fn inner(self) -> std::alloc::Layout {
        self.0
    }
}

/// Build the layout for `#[repr(C)]` based on field types. This macro
/// takes an "array" of types calculates de layout for each type
/// then combines them with [`Layout::for_repr_c`] to build the final
/// struct layout. The macro also returns the offset in bytes
/// for each field, both results are inside a [`Result`]. This
/// macro only returns an error on arithmetic overflow.
///
/// # Basic usage
///
/// ```
/// use dsrs::mem::Layout;
/// use dsrs::layout_repr_c;
///
/// #[repr(C)]
/// struct Struct<T> {
///     id: u8,
///     data: T
/// }
///
/// let (layout, offsets) = layout_repr_c!([u8, f32]).unwrap();
/// // this is equivalent to
/// let (layout_, offsets_) = Layout::for_repr_c([Layout::new::<u8>(), Layout::new::<f32>()]).unwrap();
///
/// assert_eq!(layout, Layout::new::<Struct<f32>>());
/// # assert_eq!((layout, offsets), (layout_, offsets_));
/// ```
///
/// # Unsized Types
///
/// For [exotically sized types](https://doc.rust-lang.org/nomicon/exotic-sizes.html)
/// the layout can't be know just from the type, so it is necessary
/// the actual value present at the field. Since only the last field
/// of a struct can be unsized, the macro receives all [`Sized`]
/// fields normally, then the last one comes after a semicolon (`;`)
/// with the format `Type = value`.
///
/// ```
/// # use dsrs::mem::Layout;
/// # use dsrs::layout_repr_c;
/// #
/// #[repr(C)]
/// struct Unsized {
///     some_num: f64,
///     dst: str
/// }
///
/// // layout for an `Unsized` with "string" in field `Unsized::dst`
/// let (layout, offsets) = layout_repr_c!([f64; str = "string"]).unwrap();
/// #
/// # assert_eq!(offsets, [0, 8]);
/// ```
///
/// Sometimes, the actual struct contains only one field, wich may be
/// unsized. In this case, the semicolon is not necessary.
///
/// ```
/// # use dsrs::mem::Layout;
/// # use dsrs::layout_repr_c;
/// use std::fmt::Debug;
///
/// #[repr(C)]
/// struct Dynamic {
///     dst: dyn Debug
/// }
///
/// let (layout, offsets) = layout_repr_c!([dyn Debug = 2i32]).unwrap();
/// // the semicolon may apper, if desired
/// let (layout_, offsets_) = layout_repr_c!([; dyn Debug = 2i32]).unwrap();
///
/// // for this specific `Dynamic` object, this layout is equivalent
/// assert_eq!((layout, offsets), layout_repr_c!([i32]).unwrap());
/// # assert_eq!((layout, offsets), (layout_, offsets_));
/// ```
///
/// For ZSTs, the usage is straightforward.
///
/// ```
/// # use dsrs::mem::Layout;
/// # use dsrs::layout_repr_c;
/// #
/// #[repr(C)]
/// struct ZST { }
///
/// let (layout, offsets) = layout_repr_c!([]).unwrap();
///
/// assert_eq!(layout, Layout::new::<ZST>());
/// assert_eq!(offsets, []);
/// ```
///
/// For empty types, no layout will ever make sense.
///
/// # Specifying only the layout or the offsets
///
/// Normally, the macro returns both the layout for the struct
/// and the offset of its fields, just like [`Layout::for_repr_c`].
/// If it is desired only one or another, it can be specified
/// before the array with the field types.
///
/// ```
/// # use dsrs::mem::Layout;
/// # use dsrs::layout_repr_c;
/// #
/// #[repr(C)]
/// struct Struct {
///     x: f32,
///     y: i128
/// }
///
/// let (layout, offsets) = layout_repr_c!([f32, i128]).unwrap();
///
/// // only the layouts
/// let layout_ = layout_repr_c!(layout [f32, i128]).unwrap();
/// // only the offsets
/// let offsets_ = layout_repr_c!(offsets [f32, i128]).unwrap();
/// #
/// # assert_eq!(layout, layout_);
/// # assert_eq!(offsets, offsets_);
/// ```
#[macro_export]
macro_rules! layout_repr_c {
    // get only the layout
    (layout $fields: tt) => {
        match layout_repr_c!($fields) { Err(err) => Err(err), Ok((layout, _)) => Ok(layout) }
    };
    // get only the offsets
    (offsets $fields: tt) => {
        match layout_repr_c!($fields) { Err(err) => Err(err), Ok((_, offsets)) => Ok(offsets) }
    };
    // from array of `Sized` types
    ([$($field: ty),*]) => {
        Layout::for_repr_c([$( Layout::new::<$field>(), )*])
    };
    // only one unsized type
    ([$last: ty = $value: expr]) => {
        Layout::for_repr_c([Layout::for_value::<$last>(&$value)])
    };
    // unsized struct, that is, struct with unsized last field
    ([$($field: ty),*; $last: ty = $value: expr]) => {
        Layout::for_repr_c([$( Layout::new::<$field>(), )* Layout::for_value::<$last>(&$value)])
    };
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

    #[test]
    fn unsized_repr_c() {
        #[derive(Debug, Eq, PartialEq)]
        #[repr(C)]
        struct Unsized {
            first: u64,
            other: [u64]
        }

        impl PartialEq<[u64]> for Unsized {
            fn eq(&self, other: &[u64]) -> bool {
                match other {
                    [first, rest @ ..] => &self.first == first && &self.other == rest,
                    [] => false
                }
            }
        }

        macro_rules! len {
            ($ptr:ident) => {
                &mut *(&mut $ptr as *mut _ as *mut usize).offset(1)
            };
        }

        let values = [5, 1, 2, 3, 4];
        let mut ptr = &values as &[u64];
        assert_eq!(values.len(), unsafe { *len!(ptr) });

        let val = unsafe {
            let val: Box<[u64]> = Box::new(values);
            let mut ptr = Box::into_raw(val);
            *len!(ptr) = *len!(ptr) - 1;

            Box::from_raw(ptr as *mut Unsized)
        };
        assert_eq!(val.as_ref(), &values as &[u64]);

        let fields = [Layout::new::<u64>(), Layout::for_value(&values[1..])];
        assert_eq!(Layout::for_repr_c(fields), Ok((Layout::for_value(val.as_ref()), [0, 8])))
    }

    #[test]
    fn repr_c_macro() {
        #[repr(C)]
        struct Struct {
            a: u32,
            b: u32
        }
        #[repr(C)]
        struct Int {
            num: i32
        }

        assert_eq!(Ok((Layout::new::<()>(), [])), layout_repr_c!([]));
        assert_eq!(Ok((Layout::new::<Int>(), [0])), layout_repr_c!([i32]));

        assert_eq!(Ok((Layout::new::<Int>(), [0])), layout_repr_c!([i32 = 2]));
        assert_eq!(Ok((Layout::new::<Int>(), [0])), layout_repr_c!([; i32 = 2]));
        assert_eq!(Ok((Layout::new::<Struct>(), [0, 4])), layout_repr_c!([u32; u32 = 3]));

        assert_eq!(Ok(Layout::new::<()>()), layout_repr_c!(layout []));
        assert_eq!(Ok(Layout::new::<Int>()), layout_repr_c!(layout [i32]));
        assert_eq!(Ok(Layout::new::<Struct>()), layout_repr_c!(layout [u32, u32]));

        assert_eq!(Ok([]), layout_repr_c!(offsets []));
        assert_eq!(Ok([0]), layout_repr_c!(offsets [i32]));
        assert_eq!(Ok([0, 4]), layout_repr_c!(offsets [u32, u32]));

        assert_eq!(Ok(Layout::new::<Int>()), layout_repr_c!(layout [i32 = 2]));
        assert_eq!(Ok(Layout::new::<Struct>()), layout_repr_c!(layout [u32; u32 = 3]));
        assert_eq!(Ok([0]), layout_repr_c!(offsets [i32 = 2]));
        assert_eq!(Ok([0, 4]), layout_repr_c!(offsets [u32; u32 = 3]));
    }
}
