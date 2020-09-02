//! Pointers that can never be null.

use std::fmt::{Debug, Pointer, Formatter, Result};
use std::hash::{Hash, Hasher};
use std::cmp::{Ord, PartialOrd, Ordering};
use std::ops::{CoerceUnsized, DispatchFromDyn};
use std::marker::Unsize;
use hint::likely;

/// `*mut T` but non-zero and covariant.
///
/// This is a wrapper for [`std::ptr::NonNull`], which makes
/// [`from`](NonNull::from), [`as_ref`](NonNull::as_ref) and
/// [`as_mut`](NonNull::as_mut) as `const`, along with a few
/// other methods.
#[repr(transparent)]
pub struct NonNull<T: ?Sized>(pub std::ptr::NonNull<T>);


impl<T: ?Sized> NonNull<T> {
    /// Creates a new `NonNull`.
    ///
    /// See [`std::ptr::NonNull::new_unchecked`].
    ///
    /// # Safety
    ///
    /// `ptr` must be non-null.
    #[inline]
    pub const unsafe fn new_unchecked(ptr: *mut T) -> Self {
        Self(std::ptr::NonNull::new_unchecked(ptr))
    }

    /// Creates a new `NonNull` if `ptr` is non-null.
    ///
    /// See [`std::ptr::NonNull::new`]
    #[allow(clippy::not_unsafe_ptr_arg_deref)]
    #[inline]
    pub const fn new(ptr: *mut T) -> Option<Self> {
        if likely!(!ptr.is_null()) {
            // SAFETY: already checked for null
            Some(unsafe { Self::new_unchecked(ptr) })
        } else {
            None
        }
    }

    /// Acquires the underlying `*mut` pointer.
    ///
    /// See [`std::ptr::NonNull::as_ptr`].
    #[inline]
    pub const fn as_ptr(self) -> *mut T {
        self.0.as_ptr()
    }

    /// Returns a shared reference to the value. If the value may be uninitialized, [`as_uninit_ref`]
    /// must be used instead.
    ///
    /// For the mutable counterpart see [`as_mut`].
    ///
    /// [`as_uninit_ref`]: #method.as_uninit_ref
    /// [`as_mut`]: #method.as_mut
    ///
    /// # Safety
    ///
    /// When calling this method, you have to ensure that all of the following is true:
    ///
    /// * The pointer must be properly aligned.
    ///
    /// * It must be "dereferencable".
    ///
    /// * The pointer must point to an initialized instance of `T`.
    ///
    /// * You must enforce Rust's aliasing rules, since the returned lifetime `'a` is
    ///   arbitrarily chosen and does not necessarily reflect the actual lifetime of the data.
    ///
    /// This applies even if the result of this method is unused!
    ///
    /// See [`std::ptr::NonNull::as_ref`].
    #[inline]
    pub const unsafe fn as_ref(&self) -> &T {
        &*(self.0.as_ptr() as *const T)
    }

    /// Returns a unique reference to the value. If the value may be uninitialized, [`as_uninit_mut`]
    /// must be used instead.
    ///
    /// For the shared counterpart see [`as_ref`].
    ///
    /// [`as_uninit_mut`]: #method.as_uninit_mut
    /// [`as_ref`]: #method.as_ref
    ///
    /// # Safety
    ///
    /// When calling this method, you have to ensure that all of the following is true:
    ///
    /// * The pointer must be properly aligned.
    ///
    /// * It must be "dereferencable".
    ///
    /// * The pointer must point to an initialized instance of `T`.
    ///
    /// * You must enforce Rust's aliasing rules, since the returned lifetime `'a` is
    ///   arbitrarily chosen and does not necessarily reflect the actual lifetime of the data..
    ///
    /// This applies even if the result of this method is unused!
    ///
    /// See [`std::ptr::NonNull::as_mut`].
    #[inline]
    pub const unsafe fn as_mut(&mut self) -> &mut T {
        &mut *self.0.as_ptr()
    }

    /// Casts to a pointer of another type.
    ///
    /// See [`std::ptr::NonNull::cast`].
    #[inline]
    pub const fn cast<U>(self) -> NonNull<U> {
        NonNull(self.0.cast())
    }

    /// Creates a new `NonNull` from a reference.
    ///
    /// Since a valid reference is never null, this is always safe.
    /// This is also conceptually equivalent to `&value as *const T
    /// as *mut T`, which is safe and guaranteed to be non null.
    ///
    /// Note: implemented as a `const` method intead of implementing
    /// the trait [`From`].
    ///
    /// # Example
    ///
    /// ```
    /// # use mem::ptr::NonNull;
    /// #
    /// let mut x = 2;
    /// let nonnull = NonNull::from(&x);
    ///
    /// assert_eq!(nonnull.as_ptr(), &mut x as *mut _);
    /// ```
    #[inline]
    pub const fn from(value: &T) -> Self {
        let ptr = value as *const T as *mut T;
        // SAFETY: a reference is never null
        unsafe { Self::new_unchecked(ptr) }
    }

    /// Recover inner [`std::ptr::NonNull`] from `NonNull`.
    ///
    /// # Example
    ///
    /// ```
    /// # use mem::ptr::NonNull;
    /// #
    /// let val = "string";
    /// let wrapper = NonNull::from(val);
    /// let nonnull = std::ptr::NonNull::from(val);
    ///
    /// assert_eq!(wrapper.inner(), nonnull);
    /// ```
    #[inline(always)]
    pub const fn inner(self) -> std::ptr::NonNull<T> {
        self.0
    }

    /// This is true when `NonNull<T>` is a fat pointer.
    ///
    /// # Example
    ///
    /// ```
    /// # use mem::ptr::NonNull;
    /// #
    /// assert_eq!(NonNull::<[u8]>::is_fat_pointer(), true);
    /// assert_eq!(NonNull::<f32>::is_fat_pointer(), false);
    /// ```
    #[inline]
    pub const fn is_fat_pointer() -> bool {
        super::is_fat_pointer::<T>()
    }

    /// Update inner pointer.
    ///
    /// This function updates inner `*mut T` pointer keeping the
    /// metadata for fat pointers. For thin pointers, with
    /// `T: Sized`, this is equivalent to [`data.cast<T>()`](NonNull::cast).
    ///
    /// # Example
    ///
    /// For thin pointers:
    ///
    /// ```
    /// # use mem::ptr::NonNull;
    /// #
    /// let (a, b) = (2u32, 10u8);
    ///
    /// let nonnull_a = NonNull::from(&a);
    /// let nonnull_b = NonNull::from(&b);
    ///
    /// assert_eq!(nonnull_a.update(nonnull_b), nonnull_b.cast::<u32>())
    /// ```
    ///
    /// For fat pointers:
    ///
    /// ```
    /// # use mem::ptr::NonNull;
    /// #
    /// fn address_eq<T: ?Sized>(x: *const T, y: *const T) -> bool {
    ///     (x as *const ()) == (y as *const ())
    /// }
    ///
    /// let (a, b) = ("Isaac", "Kleysson");
    ///
    /// let nonnull_a = NonNull::from(a);
    /// let nonnull_b = NonNull::from(b);
    ///
    /// // points to `b` but expect length of `a`
    /// let frankstein = nonnull_a.update(nonnull_b.cast());
    ///
    /// assert!(address_eq(frankstein.as_ptr(), nonnull_b.as_ptr()));
    /// assert!(!std::ptr::eq(frankstein.as_ptr(), nonnull_a.as_ptr()));
    /// assert!(!std::ptr::eq(frankstein.as_ptr(), nonnull_b.as_ptr()));
    /// assert!(!std::ptr::eq(nonnull_a.as_ptr(), nonnull_b.as_ptr()));
    ///
    /// // the new pointer is equivalent to this slice
    /// let c = &b[..a.len()];
    /// let nonnull_c = NonNull::from(c);
    ///
    /// assert!(std::ptr::eq(frankstein.as_ptr(), nonnull_c.as_ptr()));
    /// ```
    #[inline]
    pub const fn update(self, data: NonNull<u8>) -> Self {
        let ptr = super::update_data(self.as_ptr(), data.as_ptr());
        // SAFETY: data is not null
        unsafe { NonNull::new_unchecked(ptr) }
    }

    /// Updates metadata for fat pointers.
    ///
    /// This function tries to update the metadata for a fat pointer.
    /// It returns `Some` new pointer with the updated metadata, in this
    /// case. When `*mut T` is not fat, returns `None`. No checks on
    /// metadata are made.
    ///
    /// Even when successful, the pointer still points to the same
    /// address.
    ///
    /// # Example
    ///
    /// ```
    /// # use mem::ptr::NonNull;
    /// #
    /// let sized = NonNull::from(&2);
    /// assert_eq!(sized.update_metadata(10), None);
    ///
    /// let text = "John James";
    /// let nonsized = NonNull::from(text);
    ///
    /// assert_eq!(nonsized.update_metadata(4), Some(NonNull::from(&text[..4])));
    /// ```
    #[inline]
    pub const fn update_metadata(self, metadata: usize) -> Option<Self> {
        if Self::is_fat_pointer() {
            // SAFETY: *mut T is checked as a fat pointer
            Some(unsafe { self.update_metadata_unchecked(metadata) })
        } else {
            None
        }
    }

    /// Updates metadata for fat pointers without checking.
    ///
    /// Unchecked version of [`NonNull::update_metadata`]. Works the same
    /// for fat pointers, without wrapinng the result in an
    /// `Option`.
    ///
    /// # Safety
    ///
    /// Only safe if `*mut T` is a fat pointer.
    ///
    /// If it is a thin pointer, this is undefined behaviour and could
    /// lead to write of metadata on an invalid address.
    ///
    /// # Example
    ///
    /// ```
    /// # use mem::ptr::NonNull;
    /// #
    /// let text = "Tetinha";
    /// let nonsized = NonNull::from(text);
    ///
    /// // SAFETY: `*mut str` is a fat pointer
    /// let updated = unsafe { nonsized.update_metadata_unchecked(6) };
    /// assert_eq!(Some(updated), nonsized.update_metadata(6));
    /// ```
    #[inline]
    pub const unsafe fn update_metadata_unchecked(self, metadata: usize) -> Self {
        // SAFETY: caller must ensure ptr is fat
        let ptr = super::update_metadata(self.as_ptr(), metadata);
        // SAFETY: self is not null
        NonNull::new_unchecked(ptr)
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

impl<T: ?Sized> Into<*mut T> for NonNull<T> {
    #[inline]
    fn into(self) -> *mut T {
        self.as_ptr()
    }
}

impl<T: ?Sized> Into<*const T> for NonNull<T> {
    #[inline]
    fn into(self) -> *const T {
        self.as_ptr()
    }
}

impl<T: ?Sized> Hash for NonNull<T> {
    #[inline]
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.as_ptr().hash(state)
    }
}

impl<T: ?Sized> Ord for NonNull<T> {
    #[inline]
    fn cmp(&self, other: &Self) -> Ordering {
        self.as_ptr().cmp(&other.as_ptr())
    }
}

impl<T: ?Sized> PartialOrd for NonNull<T> {
    #[inline]
    fn partial_cmp(&self, other: &Self) -> Option<Ordering> {
        self.as_ptr().partial_cmp(&other.as_ptr())
    }
}

impl<U: ?Sized, T: ?Sized + Unsize<U>> CoerceUnsized<NonNull<U>> for NonNull<T> {}

impl<U: ?Sized, T: ?Sized + Unsize<U>> DispatchFromDyn<NonNull<U>> for NonNull<T> {}

#[cfg(test)]
mod tests {
    use super::*;

    /// Guarantee that `*mut T`, `NonNull<T>` and `Option<NonNull<T>>`
    /// all have the same size, independently if `T` is an
    /// [Exotically Sized Type](https://doc.rust-lang.org/nomicon/exotic-sizes.html)
    #[test]
    fn packed_option_size() {
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

    /// Guarantees that `NonNull` methods are equivalent
    #[test]
    fn equivalent_methods() {
        use std::ptr::NonNull as Inner;

        let val = "string";
        let ptr = NonNull::from(val);

        assert_eq!(ptr.inner(), Inner::from(val));
        // SAFETY: `ptr` is a valid reference
        unsafe { assert_eq!(ptr.as_ref(), ptr.inner().as_ref()) };
        // SAFETY: `val` is not being used, so it can be mutable,
        // also `str` will not be mutated
        unsafe { assert_eq!(ptr.clone().as_mut(), ptr.inner().as_mut()) };

        let ptr = val as *const str as *mut str;
        assert_eq!(NonNull::new(ptr), Inner::new(ptr).map(NonNull));
        let null = std::ptr::null_mut::<i32>();
        assert_eq!(NonNull::new(null), Inner::new(null).map(NonNull));
    }
}
