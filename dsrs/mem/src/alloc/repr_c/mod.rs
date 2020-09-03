//! Marker and associted types in a `#[repr(C)]` struct.
mod field_tuple;

use std::alloc::handle_alloc_error;
use field_tuple::{FieldTuple, layout_with_last_field};
use super::layout::{Layout, Result};
use super::{grow, shrink, NonNull};

/// The starting fields of a `#[repr(C)]` struct `T`.
///
/// ```
/// # use mem::alloc::{ReprC, Start};
/// #
/// #[repr(C)]
/// struct Person {
///     age: u16,
///     name: str
/// }
///
/// // SAFETY: Person is a repr(C) struct
/// unsafe impl ReprC for Person {
///     type Fields = (u16, str);
/// }
///
/// fn birthday((age,): Start<Person>) -> u16 {
///     age + 1
/// }
///
/// // Start<Person> == (u16,)
/// assert_eq!(birthday((27u16,)), 28)
/// ```
pub type Start<T> = <<T as ReprC>::Fields as FieldTuple>::Start;
/// The very last field of a `#[repr(C)]` struct `T`.
///
/// ```
/// # use mem::alloc::{ReprC, Last};
/// # macro_rules! println {
/// #    ($($arg:tt)*) => { assert_eq!(format!($($arg)*), "Hello, Julian!") };
/// # }
/// #
/// #[repr(C)]
/// struct Person {
///     age: u16,
///     name: str
/// }
///
/// // SAFETY: Person is a repr(C) struct
/// unsafe impl ReprC for Person {
///     type Fields = (u16, str);
/// }
///
/// // Last<Person> == str
/// fn say_hello(name: &Last<Person>) {
///     println!("Hello, {}!", name)
/// }
///
/// say_hello("Julian")
/// // prints: Hello, Julian!
/// ```
pub type Last<T> = <<T as ReprC>::Fields as FieldTuple>::Last;

/// Types that are implemented as a `#[repr(C)]` struct.
///
/// Since Rust doesn't specify the fields layout, the only viable,
/// unpacked, specified struct representation is based on C. With
/// this representation and knowing the types of each field, it is
/// possible to build the struct in heap memory, without knowing
/// its size.
///
/// The reasoning here is exactly this, if the type is guaranteed
/// to be a `#[repr(C)]` struct, this can be implemented describing
/// the type of each field with a [tuple]. Note that
/// since only the last field of a struct may be unsized, this is
/// also required for the [`Fields`](ReprC::Fields) tuple.
///
/// [tuple]: https://doc.rust-lang.org/nightly/std/primitive.tuple.html
///
/// # Safety
///
/// This trait assumes that type is guaranteed to be `#[repr(C)]`
/// struct. For other representations, this is undefined behaviour.
///
/// Besides that, the [`Fields`](ReprC::Fields) must have the
/// same fields used in the struct, in the order they appear.
/// The type may repeat if this also happens in the struct.
///
/// The last requirement is that the last field must not be an
/// [extern type], as the compiler can't know the layout of such
/// type. (Actually, no field in the struct can be `extern`, but
/// since just the last field may be `!Sized`, this no other field
/// could be an extern type.)
///
/// [extern type]: https://doc.rust-lang.org/nightly/unstable-book/language-features/extern-types.html
///
/// # Example
///
/// ```
/// # use mem::alloc::ReprC;
/// #
/// #[repr(C)]
/// struct CStruct {
///     x: i32,
///     y: i32,
///     z: (),
///     w: String,
///     t: i32,
///     u: [char]
/// }
///
/// unsafe impl ReprC for CStruct {
///     type Fields = (i32, i32, (), String, i32, [char]);
/// }
/// ```
pub unsafe trait ReprC {
    /// Types describing the struct in order of appearence.
    ///
    /// For struct like this:
    /// ```
    /// # use mem::alloc::ReprC;
    /// use std::fmt::Debug;
    ///
    /// #[repr(C)]
    /// struct SomeData {
    ///     int: i32,
    ///     text: String,
    ///     num: f64,
    ///     dynobj: dyn Debug
    /// }
    /// ```
    ///
    /// The fields are:
    /// ```
    /// # use mem::alloc::ReprC;
    /// # use std::fmt::Debug;
    /// #
    /// # #[repr(C)]
    /// # struct SomeData {
    /// #     int: i32,
    /// #     text: String,
    /// #     num: f64,
    /// #     dynobj: dyn Debug
    /// # }
    /// #
    /// type SomeFields = (i32, String, f64, dyn Debug);
    ///
    /// // so the implementation should be
    /// unsafe impl ReprC for SomeData {
    ///     type Fields = (i32, String, f64, dyn Debug);
    /// }
    /// ```
    ///
    /// Note that only the last field may be unsized.
    type Fields: FieldTuple + ?Sized;

    /// Expands a box with the last field into a box with the full
    /// struct, inserting the other fields at the right memory position.
    ///
    /// This method permits creating a box for an unsized struct when
    /// you already have a box to its last type. It will need the
    /// values for the other fields, which are sized.
    ///
    /// This method will also avoid reallocating memory when thats
    /// unnecessary, instead using the memory of the already boxed
    /// last value, which may be most of the size of the struct.
    /// Although reallocation is avoided, the last value will
    /// probably need to be copied at least a few bytes ahead,
    /// which might be slow for large values of [`Last<Self>`](Last).
    /// If this is undesired, it is better to use a sized struct
    /// holding the box to the last field.
    ///
    /// # Example
    ///
    /// ```
    /// use mem::alloc::ReprC;
    ///
    /// #[repr(C)]
    /// struct Unique<T: ?Sized> {
    ///     id: usize,
    ///     data: T
    /// }
    ///
    /// // SAFETY: Unique<T> is a repr(C) struct
    /// unsafe impl<T: ?Sized> ReprC for Unique<T> {
    ///     type Fields = (usize, T);
    /// }
    ///
    /// impl<T: ?Sized> Unique<T> {
    ///     fn new(id: usize, data: Box<T>) -> Box<Self> {
    ///         Self::expand(data, (id,))
    ///     }
    /// }
    ///
    /// let name = String::from("Hannah Montana");
    /// let unique = Unique::new(12, name.into_boxed_str());
    ///
    /// assert_eq!(unique.id, 12);
    /// let words: Vec<_> = unique.data.split_whitespace().collect();
    /// assert_eq!(words, vec!["Hannah", "Montana"])
    /// ```
    #[inline]
    fn expand(last: Box<Last<Self>>, fields: Start<Self>) -> Box<Self> {
        let last_ptr = Box::into_raw(last);

        // SAFETY: since `last` is a reference, it is a valid pointer to `Last`
        let (self_layout, last_layout) = match unsafe { layout_with_last_field::<Self::Fields>(last_ptr) } {
            // SAFETY: again, valid reference
            Err(_) => handle_alloc_error(unsafe { Layout::for_value_raw(last_ptr).inner() }),
            Ok((layout, _, last)) => (layout, last)
        };

        // SAFETY: a box must always be allocated via the global allocator and the poiner was nonnull
        let (self_ptr, last_ptr) = match unsafe { grow(NonNull::new_unchecked(last_ptr), last_layout, self_layout) } {
            Err(_) => handle_alloc_error(self_layout.inner()),
            // SAFETY: pointer now can hold a Self, but its uninitialized,
            // the metadata must also be the same, as Last is its last field
            Ok(ptr) => (unsafe { ptr.cast_unsized::<Self>() }.as_ptr(), ptr.as_ptr())
        };

        // SAFETY: self_ptr can hold Self, which is a repr(C) with Self::Fields
        // also `last` is valid since it was a reference
        unsafe {
            // move the last field to the end
            Self::Fields::write_last(self_ptr as *mut u8, last_ptr);
            // insert starting fields
            Self::Fields::write_start(self_ptr as *mut u8, fields);
        }
        // SAFETY: self_ptr is now valid and initialized reference to Self
        let this = unsafe { Box::from_raw(self_ptr) };
        // check validity of layouts
        debug_assert!(self_layout == Layout::for_value(this.as_ref()));
        this
    }

    /// Splits a boxed `Self` into its first fields and the last field.
    ///
    /// The last field is boxed as it may be dynamically sized. When `try_shrink` is `false`,
    /// the field is just moved to the start of the struct and no reallocation happens.
    ///
    /// Otherwise, when `try_shrink` is `false`, this will try to reduce memory usage
    /// by reallocationg to a smaller memory block, when necessary.
    ///
    /// Just like with [`expand`](ReprC::expand), the last value will probably need to be copied
    ///  in a overlapping way, which might be slow for large values of [`Last<Self>`](Last).
    /// If this is not desired, it is better implement the struct with `Box<T>` as its
    /// last field.
    ///
    /// # Example
    ///
    /// ```
    /// use mem::alloc::ReprC;
    ///
    /// #[repr(C)]
    /// struct Unique<T: ?Sized> {
    ///     id: usize,
    ///     data: T
    /// }
    ///
    /// // SAFETY: Unique<T> is a repr(C) struct
    /// unsafe impl<T: ?Sized> ReprC for Unique<T> {
    ///     type Fields = (usize, T);
    /// }
    ///
    /// impl<T: ?Sized> Unique<T> {
    ///     fn new(id: usize, data: Box<T>) -> Box<Self> {
    ///         Self::expand(data, (id,))
    ///     }
    ///
    ///     fn into_data(self: Box<Self>) -> Box<T> {
    ///         let (boxed, (_,)) = Self::split(self, false);
    ///         boxed
    ///     }
    /// }
    ///
    /// let name = String::from("Hannah Montana");
    /// let unique = Unique::new(12, name.clone().into_boxed_str());
    ///
    /// assert_eq!(unique.id, 12);
    /// let u_name = String::from(unique.into_data());
    /// assert_eq!(u_name, name);
    /// #
    /// # let unique = Unique::new(12, u_name.into_boxed_str());
    /// # assert_eq!(&unique.data, name);
    /// # assert_eq!(unique.id, 12)
    /// ```
    #[inline]
    fn split(self: Box<Self>, try_shrink: bool) -> (Box<Last<Self>>, Start<Self>) {
        let self_layout = Layout::for_value(self.as_ref());
        let self_ptr = Box::into_raw(self);
        // SAFETY: since self is initialized, it can hold its Last field
        // but the pointer might be to another field
        let mut last_ptr = unsafe { *(&self_ptr as *const _ as *const *mut Last<Self>) };

        // SAFETY: NOT SURE: Self can hold Last<Self>, the metadata is correct and, since
        // Self is a #[repr(C)] struct, the pointer is aligned correctly, but it might not
        // point to any valid Last object
        let last_layout = match unsafe { layout_with_last_field::<Self::Fields>(last_ptr) } {
            Err(_) => handle_alloc_error(self_layout.inner()),
            Ok((this_layout, _, layout)) => {
                // check validity of layouts
                debug_assert!(this_layout == self_layout);
                layout
            }
        };

        // SAFETY: last_ptr points to an initialized object of Self, so the start fields are safe to read
        let start = unsafe { Self::Fields::read_start(last_ptr as *const u8) };
        // SAFETY: NOT SURE: Self can hold Last<Self>, the metadata is correct and, since
        // Self is a #[repr(C)] struct, the pointer is aligned correctly, but it might not
        // point to any valid Last object
        unsafe { Self::Fields::read_last(self_ptr as *const u8, last_ptr); }


        if try_shrink {
            // SAFETY: a box must always be allocated via the global allocator and the poiner was nonnull
            match unsafe { shrink(NonNull::new_unchecked(self_ptr), self_layout, last_layout) } {
                Err(_) => handle_alloc_error(last_layout.inner()),
                Ok(new) => last_ptr = unsafe { new.cast_unsized().as_ptr() }
            }
        } else {
            debug_assert!(self_layout.align() >= last_layout.align())
        }
        // SAFETY: last_ptr now points to the start of of the last field, shrunk or not
        let last = unsafe { Box::from_raw(last_ptr) };

        // check validity of layouts
        debug_assert!(last_layout == Layout::for_value(last.as_ref()));
        (last, start)
    }
}
