//! Associated types for the fields in a `#[repr(C)]` struct.

use super::{Layout, Result};


/// Build the layout for the equivalent `#[repr(C)]` struct.
#[inline]
pub(super) const unsafe fn layout_with_last_field<T: FieldTuple + ?Sized>(val: *const T::Last) -> Result<(Layout, usize, Layout)> {
    let last_layout = Layout::for_value_raw(val);

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
    unsafe fn write_start(ptr: *mut u8, start: Self::Start) -> Result<()>;

    /// Reads contents of memory location with the starting fields of a `#[repr(C)]` struct.
    ///
    /// This read at right offsets, following required alignment.
    /// The values at `start` tuple are considered owned now.
    unsafe fn read_start(ptr: *const u8) -> Result<Self::Start>;

    #[inline]
    /// Write the last field of an equivalent `#[repr(C)]` struct at the right offset.
    ///
    /// Memory may overlap.
    unsafe fn write_last(ptr: *mut u8, last: *const Self::Last) -> Result<()> {
        // get offset and layout for last field
        let (_, offset, layout) = layout_with_last_field::<Self>(last)?;

        // pointer to last field in struct
        let data_ptr = ptr.add(offset);
        // must be aligned to `Self::Last`
        debug_assert!(layout.is_aligned(data_ptr) && layout.is_aligned(last));

        // copy the last field, which may overlap
        std::ptr::copy(last as *const u8, data_ptr, layout.size());
        Ok(())
    }

    #[inline]
    /// Read the last field of an equivalent `#[repr(C)]` struct at the right offset.
    ///
    /// Memory may overlap.
    unsafe fn read_last(ptr: *const u8, last: *mut Self::Last) -> Result<()> {
        // get offset and layout for last field
        let (_, offset, layout) = layout_with_last_field::<Self>(last)?;

        // pointer to last field in struct
        let data_ptr = ptr.add(offset);
        // must be aligned to `Self::Last`
        debug_assert!(layout.is_aligned(data_ptr) && layout.is_aligned(last));

        // copy the last field, which may overlap
        std::ptr::copy(data_ptr, last as *mut u8, layout.size());
        Ok(())
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

/// Implement type list for generic tuple. The
/// very last type identifier might be unsized.
///
/// `impl_field_tuple(A, B; C)` will impl trait for
/// `(A, B, C)` with `<A, B, C: ?Sized>`.
macro_rules! impl_field_tuple {
    // implementation for unit
    () => {
        unsafe impl FieldTuple for () {
            const ARITY: usize = count!();
            type Start = ();
            /// There is no last type.
            type Last = !;
            const START_LAYOUT: Layout = layout_start!();

            #[inline]
            unsafe fn write_start(ptr: *mut u8, _: ()) -> Result<()> {
                // just to check alignment
                std::ptr::write(ptr as *mut (), ());
                Ok(())
            }

            #[inline]
            unsafe fn read_start(ptr: *const u8) -> Result<()> {
                // just to check alignment
                std::ptr::read(ptr as *const ());
                Ok(())
            }

            /// Should never be called, as never type should not exists.
            #[inline]
            unsafe fn write_last(_: *mut u8, _: *const !) -> Result<()> {
                panic!("pointer to never type")
            }

            /// Should never be called, as never type should not exists.
            #[inline]
            unsafe fn read_last(_: *const u8, _: *mut !) -> Result<()> {
                panic!("pointer to never type")
            }
        }
    };
    // implementation for inhabited tuples
    ($($type: ident $name: ident),*; $last: ident) => {
        unsafe impl<$($type,)* $last: ?Sized> FieldTuple for ($($type,)* $last,) {
            const ARITY: usize = count!($($type,)* $last);
            type Start = ($($type,)*);
            type Last = $last;
            const START_LAYOUT: Layout = layout_start!($($type),*);

            #[inline]
            unsafe fn write_start(ptr: *mut u8, ($($name,)*): ($($type,)*)) -> Result<()> {
                // just to check alignment
                std::ptr::write(ptr as *mut (), ());

                #[allow(unused_mut, unused)]
                let mut layout = Layout::EMPTY;
                $(
                        // calculates the field offset while extending the layout
                        let (new_layout, offset) = layout.extend(Layout::new::<$type>())?;
                        // adjust pointer to field position
                        let data_ptr = ptr.add(offset) as *mut $type;
                        // write field
                        std::ptr::write(data_ptr, $name);

                        #[allow(unused_assignments)]
                        // update layout
                        layout = new_layout;
                )*

                Ok(())
            }

            #[inline]
            unsafe fn read_start(ptr: *const u8) -> Result<($($type,)*)> {
                // just to check alignment
                std::ptr::read(ptr as *const ());

                #[allow(unused_mut, unused)]
                let mut layout = Layout::EMPTY;
                $(
                        // calculates the field offset while extending the layout
                        let (new_layout, offset) = layout.extend(Layout::new::<$type>())?;
                        // adjust pointer to field position
                        let data_ptr = ptr.add(offset) as *mut $type;
                        // write field
                        let $name = std::ptr::read(data_ptr);

                        #[allow(unused_assignments)]
                        // update layout
                        layout = new_layout;
                )*

                Ok(($($name,)*))
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
    // start recursing, impl unit
    ($($type: ident $name: ident),*) => {
        impl_field_tuple!{}

        impl_acc!{[$($type $name),*] => []}
    }
}

// `FieldTuple` implementation for up to arity 26
impl_acc!(
    A a, B b, C c, D d, E e, F f, G g, H h, I i, J j, K k, L l, M m,
    N n, O o, P p, Q q, R r, S s, T t, U u, V v, W w, X x, Y y, Z z
);
