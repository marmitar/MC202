//! Raw pointer management.

use std::mem::size_of;

/// Size in bytes of a thin pointer, ie. for any `T: Sized` this is the size
/// of a `*mut T`.
pub const POINTER_SIZE: usize = size_of::<*mut u8>();
/// Size in bytes of a fat pointer.
///
/// Normally, a fat pointer is twice as large as a thin pointer. This size
/// is only for those kinds of fat pointers.
///
/// # Guarantees
///
/// ```
/// # use mem::ptr::{FAT_POINTER_SIZE, POINTER_SIZE};
/// #
/// # assert_ne!(FAT_POINTER_SIZE, POINTER_SIZE);
/// assert_eq!(FAT_POINTER_SIZE, std::mem::size_of::<*mut [u32]>());
/// assert_eq!(FAT_POINTER_SIZE, std::mem::size_of::<*const dyn Drop>());
/// assert_eq!(FAT_POINTER_SIZE, 2 * POINTER_SIZE)
/// ```
pub const FAT_POINTER_SIZE: usize = size_of::<*mut [u8]>();

/// If `*mut T` and `*const T` are thin pointers. All [`Sized`] types
/// should have thin pointers.
///
/// Some DSTs like `extern` types ([RFC 1861][rfc]) are also thin pointers.
///
/// # Example
///
/// ```
/// use mem::ptr::is_thin_pointer;
///
/// assert_eq!(is_thin_pointer::<[u8]>(), false);
/// assert_eq!(is_thin_pointer::<f32>(), true);
/// ```
///
/// [rfc]: https://github.com/rust-lang/rfcs/blob/master/text/1861-extern-types.md
#[inline]
pub const fn is_thin_pointer<T: ?Sized>() -> bool {
    size_of::<*mut T>() == POINTER_SIZE
}

/// If `*mut T` and `*const T` are fat pointers.
///
/// Normally, slices and trait objects are fat pointers.
///
/// # Example
///
/// ```
/// use mem::ptr::is_fat_pointer;
///
/// assert_eq!(is_fat_pointer::<[u8]>(), true);
/// assert_eq!(is_fat_pointer::<f32>(), false);
/// ```
///
/// # Note
///
/// This considers fat pointer as twice as large as thin pointers.
/// If there is another kind of pointer, say one that is zero sized
/// or thrice as large they will not be considered fat pointers
/// not thin pointers.
#[inline]
pub const fn is_fat_pointer<T: ?Sized>() -> bool {
    size_of::<*mut T>() == FAT_POINTER_SIZE
}

/// Updates pointer, thin or fat.
///
/// For thin pointers, especially when `T: Sized`, the returned pointer is just
/// `new_data`. But for fat pointers, this inserts the new data pointer in `ptr`
/// keeping the metadata.
///
/// This is always safe as it's just pointer arithmetic, but the result may
/// still not be a valid reference.
///
/// # Example
///
/// ```
/// use std::alloc::{Layout, realloc};
/// use std::mem::{MaybeUninit, needs_drop};
/// use mem::ptr::{update_data, update_metadata};
///
/// fn grow_box<T>(slice: Box<[T]>, capacity: usize) -> Result<Box<[MaybeUninit<T>]>, Box<[T]>> {
///     // shrinking may lead to resource leaking without
///     // it's destructor ever being run
///     assert!(slice.len() <= capacity);
///
///     let len = slice.len();
///     let ptr = Box::into_raw(slice);
///
///     // layout for reallocation
///     let layout = Layout::array::<T>(len).unwrap();
///     let new_size = Layout::array::<T>(capacity).unwrap().size();
///
///     // ptr must have been allocated before
///     assert_ne!(layout.size(), 0);
///     // try reallocating
///     let new_ptr = unsafe { realloc(ptr as *mut u8, layout, new_size) };
///
///     // allocation error
///     if new_ptr.is_null() {
///         // returns old box
///         return Err(unsafe { Box::from_raw(ptr) })
///     }
///
///     // update the pointer
///     let ptr = update_data(ptr, new_ptr) as *mut [MaybeUninit<T>];
///     // and its metadata
///     let ptr = unsafe { update_metadata(ptr, capacity) };
///     // rebuild box
///     Ok(unsafe { Box::from_raw(ptr) })
/// }
///
/// let vec = vec![1, 2, 3].into_boxed_slice();
///
/// let new_len = match grow_box(vec, 150) {
///     Err(_) => panic!("allocation error"),
///     Ok(uninit) => uninit.len()
/// };
///
/// assert_eq!(new_len, 150)
/// ```
#[inline]
pub const fn update_data<T: ?Sized>(mut ptr: *mut T, new_data: *mut u8) -> *mut T {
    let data = &mut ptr as *mut *mut T as *mut *mut u8;
    unsafe { *data = new_data; }
    ptr
}

/// Updates metada for a fat pointer.
///
/// If `*mut T` is a fat pointer, as described in [`is_fat_pointer`], this
/// updates the pointer metadata. The metadata is useful for same DSTs:
/// for [`slice`](std::slice) types it is the length of the slice and for
/// trait objects, it is the [virtual method table][vtable] (*vtable*),
/// which is actually a pointer, but can be treated as a `usize` nonetheless.
///
/// The final pointer is still the same, ie. the data pointed to continues
/// the same, if it actually points to anything.
///
/// [vtable]: https://en.wikipedia.org/wiki/Virtual_method_table
///
/// # Safety
///
/// This function is only ever safe if `*mut T` is a fat pointer. Even in this
/// case, the metadata is only meanigful if the type is known and the pointer
/// is a valid reference.
///
/// In case the pointer is **not** fat, calling this function is undefined
/// behavior because it could write a completely invalid address, while
/// trying to change the pointer metadata.
///
/// Besides that, changing the metadata of any fat pointer could lead to wildly
/// invalid references even when its type and metadata meaning are known.
///
/// # Example
///
/// Take a look at [`update_data`] for an example.
#[inline]
pub const unsafe fn update_metadata<T: ?Sized>(mut ptr: *mut T, metadata: usize) -> *mut T {
    let data = &mut ptr as *mut *mut T as *mut *mut u8;
    let meta = data.offset(1) as *mut usize;
    *meta = metadata;
    ptr
}
