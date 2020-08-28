use std::fmt::{Debug, Pointer, Formatter, Result};
use std::hash::{Hash, Hasher};
use std::cmp::Ordering;


/// Wrapper for [`std::ptr::NonNull`]
///
/// This wrapper makes [`from`](NonNull::from), [`as_ref`](NonNull::as_ref)
/// and [`as_mut`](NonNull::as_mut) as `const`.
#[repr(transparent)]
pub struct NonNull<T: ?Sized>(pub std::ptr::NonNull<T>);


impl<T: ?Sized> NonNull<T> {
    /// Read [`std::ptr::NonNull::new_unchecked`]
    #[inline]
    pub const unsafe fn new_unchecked(ptr: *mut T) -> Self {
        Self(std::ptr::NonNull::new_unchecked(ptr))
    }

    /// Creates a new `NonNull` from a reference
    ///
    /// Since a valid reference is never null, this is always safe.
    /// This is also conceptually equivalent to `value as
    /// *const T *mut T`, which is safe and garanteed to be non null.
    ///
    /// Note: implemented as method so that it can be `const`
    #[inline]
    pub const fn from(value: &T) -> Self {
        let ptr = value as *const T as *mut T;
        // SAFETY: a reference is never null
        unsafe { Self::new_unchecked(ptr) }
    }

    /// Read [`std::ptr::NonNull::new`]
    #[inline]
    pub const fn new(ptr: *mut T) -> Option<Self> {
        if ! ptr.is_null() {
            // SAFETY: already checked for null
            Some(unsafe { Self::new_unchecked(ptr) })
        } else {
            None
        }
    }

    /// Read [`std::ptr::NonNull::as_ptr`]
    #[inline]
    pub const fn as_ptr(self) -> *mut T {
        self.0.as_ptr()
    }

    /// Read [`std::ptr::NonNull::as_ref`]
    #[inline]
    pub const unsafe fn as_ref(&self) -> &T {
        &*(self.0.as_ptr() as *const T)
    }

    /// Read [`std::ptr::NonNull::as_mut`]
    #[inline]
    pub const unsafe fn as_mut(&mut self) -> &mut T {
        &mut *self.0.as_ptr()
    }

    /// Transmute `NonNull` into inner [`std::ptr::NonNull`]
    #[inline]
    pub const fn inner(self) -> std::ptr::NonNull<T> {
        self.0
    }

    /// Read [`std::ptr::NonNull::cast`]
    #[inline]
    pub const fn cast<U>(self) -> NonNull<U> {
        NonNull(self.0.cast())
    }
}

impl<T: ?Sized> Clone for NonNull<T> {
    #[inline]
    fn clone(&self) -> Self {
        Self(self.0)
    }

    #[inline(always)]
    fn clone_from(&mut self, other: &Self) {
        self.0 = other.0
    }
}
impl<T: ?Sized> Copy for NonNull<T> {}

impl<T: ?Sized> PartialEq for NonNull<T> {
    #[inline]
    fn eq(&self, other: &Self) -> bool {
        self.0 == other.0
    }
}
impl<T: ?Sized> Eq for NonNull<T> {}

impl<T: ?Sized> Ord for NonNull<T> {
    #[inline]
    fn cmp(&self, other: &Self) -> Ordering {
        self.0.cmp(&other.0)
    }
}
impl<T: ?Sized> PartialOrd for NonNull<T> {
    #[inline]
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        self.0.partial_cmp(&other.0)
    }
}

impl<T: ?Sized> Debug for NonNull<T> {
    #[inline]
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        Debug::fmt(&self.inner(), f)
    }
}

impl<T: ?Sized> Pointer for NonNull<T> {
    #[inline]
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        Pointer::fmt(&self.inner(), f)
    }
}

impl<T: ?Sized> Hash for NonNull<T> {
    #[inline]
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.inner().hash(state)
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    /// Garantee that `*mut T`, `NonNull<T>` and `Option<NonNull<T>>`
    /// all have the same size, independently if `T` is an
    /// [Exotically Sized Type](https://doc.rust-lang.org/nomicon/exotic-sizes.html)
    #[test]
    fn packed_option() {
        use std::fmt::Debug;
        use std::mem::size_of;

        enum Void {}

        fn assert_size<T: ?Sized>(spec: &str) {
            eprintln!("Packed option: {}", spec);

            assert_eq!(size_of::<*mut T>(), size_of::<Option<NonNull<T>>>());
            assert_eq!(size_of::<NonNull<T>>(), size_of::<Option<NonNull<T>>>())
        }

        assert_size::<u32>("normal type");
        assert_size::<()>("zero sized type");
        assert_size::<dyn Debug>("trait object");
        assert_size::<str>("slice type");
        assert_size::<Void>("empty type")
    }
}
