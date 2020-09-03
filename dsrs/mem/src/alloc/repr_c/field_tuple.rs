//! Associated types for the fields in a `#[repr(C)]` struct.
use super::{Layout, Result};
use std::intrinsics::assert_inhabited;

/// Build the layout for the equivalent `#[repr(C)]` struct.
///
/// # Safety
///
/// This function is always safe if `val` is a valid reference.
///
/// # Error
///
/// This will error if an arithmetic overflow happens or if the layout would
/// overflow when padding.
#[inline]
pub (super) const unsafe fn layout_with_last_field<T: FieldTuple + ?Sized>(val: *const T::Last) -> Result<(Layout, usize, Layout)> {
    // SAFETY: the caller must upheld restriction
    let last_layout = unsafe { Layout::for_value_raw(val) };

    let (layout, offset) = match T::START_LAYOUT.extend(last_layout) {
        Err(err) => return Err(err),
        Ok(data) => data
    };

    Ok((layout.pad_to_align(), offset, last_layout))
}

/// Tuple with generic types to used to
/// indicate the types on a struct.
/// Automatically implemented up to arity
/// 26.
///
/// # Safety
///
/// This trait should not be implemented
/// for any type. Only tuples.
pub unsafe trait FieldTuple {
    /// The number of elements in the tuple.
    const ARITY: usize;
    /// The starting (sub)tuple.
    type Start: FieldTuple + Sized;
    /// The last field on the tuple.
    type Last: ?Sized;

    /// Layout for its [`FieldTuple::Start`].
    const START_LAYOUT: Layout;

    /// Overwrites memory location with the starting fields of a `#[repr(C)]` struct.
    ///
    /// This functions writes at right offsets, following required alignment.
    /// The values at `start` tuple are not dropped.
    ///
    /// # Safety
    ///
    /// `ptr` must be *dereferenceable* to a `#[repr(C)]` struct described
    /// by this [`FieldTuple`] and be aligned to such a type. It must also be
    /// valid for writes of at least one such object.
    ///
    /// It must also be valid acording to [`std::ptr`](std::ptr#safety).
    ///
    /// Besides all that, the layout of the specified struct may
    /// never overflow, even when padded.
    unsafe fn write_start(ptr: *mut u8, start: Self::Start);

    /// Reads contents of memory location with the starting fields of a `#[repr(C)]` struct.
    ///
    /// This read at right offsets, following required alignment.
    /// The values at `start` tuple are considered owned now.
    ///
    /// # Safety
    ///
    /// `ptr` must be *dereferenceable* to a `#[repr(C)]` struct described
    /// by this [`FieldTuple`] and be aligned to such a type. It must also be
    /// valid for reads of at least one such object.
    ///
    /// It must also be valid acording to [`std::ptr`](std::ptr#safety).
    ///
    /// Besides all that, the layout of the specified struct may
    /// never overflow, even when padded.
    unsafe fn read_start(ptr: *const u8) -> Self::Start;

    /// Write the last field of an equivalent `#[repr(C)]` struct at the right offset.
    ///
    /// Memory may overlap.
    ///
    /// # Safety
    ///
    /// `ptr` must be *dereferenceable* to a `#[repr(C)]` struct described
    /// by this [`FieldTuple`] and be aligned to such a type. It must also be
    /// valid for writes of at least one such object.
    ///
    /// `last` must be a valid pointer to [`Self::Last`].
    ///
    /// Both must be valid acording to [`std::ptr`](std::ptr#safety).
    ///
    /// Besides all that, the layout of the specified struct may
    /// never overflow, even when padded.
    #[inline]
    unsafe fn write_last(ptr: *mut u8, last: *const Self::Last) {
        // get offset and layout for last field
        // SAFETY: given that `last` is dereferenceable, this should be valid
        let (offset, layout) = match unsafe { layout_with_last_field::<Self>(last) } {
            Ok((_, offset, layout)) => (offset, layout),
            // SAFETY: caller guarantees no overflow
            Err(_) => unsafe { hint::unreachable!() }
        };

        // pointer to last field in struct
        // SAFETY: if the ptr can hold the specified struct, the offset will be valid
        let data_ptr = unsafe { ptr.add(offset) };
        // must be aligned to `Self::Last`
        debug_assert!(layout.is_aligned(data_ptr) && layout.is_aligned(last));

        // copy the last field, which may overlap
        // SAFETY: caller must uphold for `last` and `data_ptr` is pointing
        // to the right offset at the struct
        unsafe { std::ptr::copy(last as *const u8, data_ptr, layout.size()) }
    }

    /// Read the last field of an equivalent `#[repr(C)]` struct at the right offset.
    ///
    /// Memory may overlap.
    ///
    /// # Safety
    ///
    /// `ptr` must be *dereferenceable* to a `#[repr(C)]` struct described
    /// by this [`FieldTuple`] and be aligned to such a type. It must also be
    /// valid for reads of at least one such object.
    ///
    /// `last` must be a valid pointer to a [`Self::Last`] object and must
    /// be writeable.
    ///
    /// Both must be valid acording to [`std::ptr`](std::ptr#safety).
    ///
    /// Besides all that, the layout of the specified struct may
    /// never overflow, even when padded.
    #[inline]
    unsafe fn read_last(ptr: *const u8, last: *mut Self::Last) {
        // get offset and layout for last field
        // SAFETY: given that `last` is dereferenceable, this should be valid
        let (offset, layout) = match unsafe { layout_with_last_field::<Self>(last) } {
            Ok((_, offset, layout)) => (offset, layout),
            // SAFETY: caller guarantees no overflow
            Err(_) => unsafe { hint::unreachable!() }
        };

        // pointer to last field in struct
        // SAFETY: if the ptr can hold the specified struct, the offset will be valid
        let data_ptr = unsafe { ptr.add(offset) };
        // must be aligned to `Self::Last`
        debug_assert!(layout.is_aligned(data_ptr) && layout.is_aligned(last));

        // copy the last field, which may overlapp
        // SAFETY: caller must uphold for `last` and `data_ptr` is pointing
        // to the right offset at the struct
        unsafe { std::ptr::copy(data_ptr, last as *mut u8, layout.size()) }
    }
}

/// Count identifiers.
macro_rules! count {
    () => { 0 };
    ($head: ident $(, $rest: ident)*) => {
        1 + count!($($rest),*)
    };
}

/// Macro generator for [`TupleField::START_LAYOUT`].
macro_rules! layout_start {
    ( $($type: ty),* ) => {
        match Layout::EMPTY.extend_many([$( Layout::new::<$type>() ),*]) {
            Ok((layout, _)) => layout,
            Err(_) => unreachable!()
        }
    };
}

/// The unit is a tuple.
unsafe impl FieldTuple for () {
    const ARITY: usize = count!();
    type Start = ();
    /// There is no last type.
    type Last = !;
    const START_LAYOUT: Layout = layout_start!();

    #[inline]
    unsafe fn write_start(ptr: *mut u8, _: ()) {
        // just to check alignment
        // SAFETY: the caller guarantees that `ptr` is
        // valid, ie. nonnull and aligned
        unsafe { std::ptr::write(ptr as *mut (), ()) }
    }

    #[inline]
    unsafe fn read_start(ptr: *const u8) {
        // just to check alignment
        // SAFETY: the caller guarantees that `ptr` is
        // valid, ie. nonnull and aligned
        unsafe { std::ptr::read(ptr as *const ()) }
    }

    /// Will always panic as never type should not exists.
    ///
    /// # Safety
    ///
    /// Always safe.
    #[inline]
    unsafe fn write_last(_: *mut u8, _: *const !) {
        // never type cannot have valid references
        // SAFETY: happily panics
        unsafe { assert_inhabited::<!>() }
    }

    /// Will always panic as never type should not exists.
    ///
    /// # Safety
    ///
    /// Always safe.
    #[inline]
    unsafe fn read_last(_: *const u8, _: *mut !) {
        // never type cannot have valid references
        // SAFETY: happily panics
        unsafe { assert_inhabited::<!>() }
    }
}

/// Implement type list for generic tuple. The
/// very last type identifier might be unsized.
///
/// `impl_field_tuple(A, B; C)` will impl trait for
/// `(A, B, C)` with `<A, B, C: ?Sized>`.
macro_rules! impl_field_tuple {
    // implementation for inhabited tuples
    ($($type: ident $name: ident),*; $last: ident) => {
        unsafe impl<$($type,)* $last: ?Sized> FieldTuple for ($($type,)* $last,) {
            const ARITY: usize = count!($($type,)* $last);
            type Start = ($($type,)*);
            type Last = $last;
            const START_LAYOUT: Layout = layout_start!($($type),*);

            #[inline]
            unsafe fn write_start(ptr: *mut u8, ($($name,)*): ($($type,)*)) {
                // just to check alignment
                // SAFETY: the caller guarantees that `ptr` is nonnull and aligned
                unsafe { std::ptr::write(ptr as *mut (), ()); }

                #[allow(unused_mut, unused)]
                let mut layout = Layout::EMPTY;
                $(
                        // calculates the field offset while extending the layout
                        let (new_layout, offset) = match layout.extend(Layout::new::<$type>()) {
                            Ok((layout, offset)) => (layout, offset),
                            // SAFETY: caller guarantees no overflow
                            Err(_) => unsafe { hint::unreachable!() }
                        };
                        // adjust pointer to field position
                        // SAFETY: if the ptr can hold the specified struct, the offset will be valid
                        let data_ptr = unsafe { ptr.add(offset) } as *mut $type;
                        // write field
                        // SAFETY: if caller guarantees that `ptr` can hold the specified struct
                        // and `data_ptr` is at the right offset, this is safe
                        unsafe { std::ptr::write(data_ptr, $name); }

                        #[allow(unused_assignments)]
                        // update layout
                        layout = new_layout;
                )*
            }

            #[inline]
            unsafe fn read_start(ptr: *const u8) -> ($($type,)*) {
                // just to check alignment
                // SAFETY: the caller guarantees that `ptr` is nonnull and aligned
                unsafe { std::ptr::read(ptr as *const ()); }

                #[allow(unused_mut, unused)]
                let mut layout = Layout::EMPTY;
                $(
                        // calculates the field offset while extending the layout
                        let (new_layout, offset) = match layout.extend(Layout::new::<$type>()) {
                            Ok((layout, offset)) => (layout, offset),
                            // SAFETY: caller guarantees no overflow
                            Err(_) => unsafe { hint::unreachable!() }
                        };
                        // adjust pointer to field position
                        // SAFETY: if the ptr can hold the specified struct, the offset will be valid
                        let data_ptr = unsafe { ptr.add(offset) } as *mut $type;
                        // write field
                        // SAFETY: if caller guarantees that `ptr` can hold the specified struct
                        // and `data_ptr` is at the right offset, this is safe
                        let $name = unsafe { std::ptr::read(data_ptr) };

                        #[allow(unused_assignments)]
                        // update layout
                        layout = new_layout;
                )*

                ($($name,)*)
            }
        }
    }
}

/// Accumulated implementation of generic tuples.
/// Impl trait for the accumulated names, then
/// insert first identifier on the accumulator
/// and repeat.
///
/// `impl_acc!{ [C, D] => [A, B] }` will call
/// `impl_field_tuple(A, B)` and
/// `impl_acc!{ [D] => [A, B, C] }`
///
/// The recursion can be started with
/// `impl_acc!{A, B, C, D}`.
macro_rules! impl_acc {
    // base case, does nothing
    ([] => [$($type: ident),*]) => {};
    // stop recursion, impl for all identifiers
    ([$head: ident $head_n: ident] => [$($type: ident $name: ident),*]) => {
        impl_field_tuple!{ $($type $name),*; $head }
    };
    // recursive step
    ([$head: ident $head_n: ident $(, $rest: ident $rest_n: ident)*] => [$($type: ident $name: ident),*]) => {
        impl_field_tuple!{ $($type $name),*; $head }

        impl_acc!{ [$($rest $rest_n),*] => [$($type $name,)* $head $head_n] }
    };
    // start recursing
    ($($type: ident $name: ident),*) => {
        impl_acc!{[$($type $name),*] => []}
    }
}

// `FieldTuple` implementation for up to arity 26
impl_acc!(
    A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m,
    N n, O o, P p, Q q, R r, S s, T t, U u, V v, W w, X x, Y y, Z z
);
