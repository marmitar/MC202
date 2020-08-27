/// Wrapper for [`std::ptr::NonNull`]
#[repr(transparent)]
#[derive(Debug, Copy, Clone, PartialEq, Eq, Hash, PartialOrd, Ord)]
pub struct NonNull<T: ?Sized>(pub std::ptr::NonNull<T>);


impl<T: ?Sized> NonNull<T> {
    /// Read [`std::ptr::NonNull::new_unchecked`]
    #[inline(always)]
    pub const unsafe fn new_unchecked(ptr: *mut T) -> Self {
        Self(std::ptr::NonNull::new_unchecked(ptr))
    }

    /// Creates a new `NonNull` from a reference
    ///
    /// Since a valid reference is never null, this is always safe.
    /// This is also conceptually equivalent to `value as *mut T`,
    /// which is safe and garanteed to be non null.
    #[inline(always)]
    pub const fn from(value: &mut T) -> Self {
        let ptr = value as *mut T;
        // SAFETY: a reference is never null
        unsafe { Self::new_unchecked(ptr) }
    }

    /// Read [`std::ptr::NonNull::new`]
    #[inline(always)]
    pub const fn new(ptr: *mut T) -> Option<Self> {
        if !ptr.is_null() {
            Some(unsafe { Self::new_unchecked(ptr) })
        } else {
            None
        }
    }

    /// Read [`std::ptr::NonNull::as_ptr`]
    #[inline(always)]
    pub const unsafe fn as_ptr(self) -> *mut T {
        self.0.as_ptr()
    }

    /// Read [`std::ptr::NonNull::as_ref`]
    #[inline(always)]
    pub const unsafe fn as_ref(&self) -> &T {
        &*(self.0.as_ptr() as *const T)
    }

    /// Read [`std::ptr::NonNull::as_mut`]
    #[inline(always)]
    pub const unsafe fn as_mut(&mut self) -> &mut T {
        &mut *self.0.as_ptr()
    }

    /// Transmute `NonNull` into inner [`std::ptr::NonNull`]
    #[inline(always)]
    pub const fn inner(self) -> std::ptr::NonNull<T> {
        self.0
    }

    /// Read [`std::ptr::NonNull::cast`]
    #[inline(always)]
    pub const fn cast<U>(self) -> NonNull<U> {
        NonNull(self.0.cast())
    }
}
