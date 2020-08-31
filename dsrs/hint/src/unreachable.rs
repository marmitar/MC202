//! Assertion for unreachable code.

/// Unsafe wrapper for [`std::unreachable`].
///
/// This is unsafe so it might be replaced
/// by [`std::hint::unreachable_unchecked`],
/// as the [`unreachable`](unreachable!) macro does.
///
/// # Safety
///
/// Calling this function directly is always safe,
/// although it will panic.
///
/// # Panics
///
/// Always panics.
#[inline(always)]
#[track_caller]
pub const unsafe fn unreachable() -> ! {
    std::unreachable!()
}

/// Indicates unreachable code.
///
/// This macro can only be called on `unsafe` contexts.
/// It also works on `const` contexts.
///
/// On debug builds this evaluates to [`unreachable::unreachable`](crate::unreachable::unreachable),
/// which safely panics and execution is aborted. On release
/// mode, this becomes [`std::hint::unreachable_unchecked`],
/// which is undefined behavior when reached.
///
/// # Safety
///
/// The same restrictions as [`unreachable_unchecked`](std::hint::unreachable_unchecked).
///
/// # Panics
///
/// If code is being compiled in debug mode, this will safely
/// panic when reached. When in release build, the execution
/// might get forcely aborted by the Operating System, without
/// a clean termination. Since reaching this is UB, take this
/// with a grain of salt, since anything else could also happen.
///
/// # Example
///
/// ```
/// #![feature(const_unreachable_unchecked)]
/// use std::num::NonZeroU32;
///
/// // subtract one from number
/// const fn sub1(num: NonZeroU32) -> u32 {
///     match num.get().checked_sub(1) {
///         Some(result) => result,
///         // SAFETY: this will never overflow
///         None => unsafe { hint::unreachable!() }
///     }
/// }
///
/// let nonzero = NonZeroU32::new(1);
/// assert_eq!(nonzero.map(sub1), Some(0));
/// ```
#[cfg(debug_assertions)]
#[macro_export]
macro_rules! unreachable {
    () => { $crate::unreachable::unreachable() };
}
#[cfg(not(debug_assertions))]
#[macro_export]
macro_rules! unreachable {
    () => { std::hint::unreachable_unchecked() };
}

#[cfg(test)]
mod tests {
    #[test]
    #[should_panic]
    fn unreachable_module_path() {
        unsafe { crate::unreachable::unreachable() }
    }

    #[cfg(debug_assertions)]
    #[test]
    #[should_panic]
    fn panics_in_debug() {
        unsafe { unreachable!() }
    }

    #[test]
    fn const_context() {
        use std::num::NonZeroU32;

        const fn sub1(num: NonZeroU32) -> u32 {
            match num.get().checked_sub(1) {
                Some(num) => num,
                None => unsafe { crate::unreachable!() }
            }
        }

        let one = NonZeroU32::new(2).map(sub1);

        assert_eq!(one, Some(1));
    }
}
