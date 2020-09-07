//! Hints and information about conditional code.

/// Informs the compiler that condition is always true.
///
/// This macro is unsafe and can be unsed in `const`
/// contexts. Equivalent to [`std::intrinsics::assume`].
///
/// If the condition is false, the behaviour is undefined.
/// Whenever this condition is encountered, the compiler
/// might assume that the condition will never happen
/// and may remove it.
///
/// # Examples
///
/// Many functions and method have and unsafe `unchecked` variant,
/// but sometimes the some checks are necessary while others aren't.
/// With `assume!(...)`, this information can be passed on to the
/// compiler, without the need to manually check anything.
///
/// ```
/// #![feature(const_alloc_layout)]
/// #![feature(const_unreachable_unchecked)]
/// use std::alloc::{Layout, LayoutErr};
///
/// const fn resize_layout(layout: Layout, new_size: usize) -> Result<Layout, LayoutErr> {
///     let align = layout.align();
///
///     // SAFETY: layout requires that `align` is a power of 2
///     unsafe { hint::assume!(align.is_power_of_two()); }
///
///     // compile now can skip the alignment check
///     Layout::from_size_align(new_size, align)
/// }
///
/// let layout = Layout::from_size_align(10, 4);
/// let new_layout = layout.and_then(|l| resize_layout(l, 20));
///
/// assert_eq!(new_layout, Layout::from_size_align(20, 4))
/// ```
///
/// This is specially usefull for informing the compiler about type guarantees.
///
/// ```
/// use std::num::NonZeroI32;
///
/// // do a / b
/// fn div(a: NonZeroI32, b: NonZeroI32) -> Option<NonZeroI32> {
///     let (a, b) = (a.get(), b.get());
///
///     // compiler can skip the check that b is zero
///     unsafe { hint::assume!(b != 0) }
///     // but still needs to check for overflow
///     a.checked_div(b).and_then(NonZeroI32::new)
/// }
///
/// let minus_one = NonZeroI32::new(-1).unwrap();
/// let one = NonZeroI32::new(1).unwrap();
/// let two = NonZeroI32::new(2).unwrap();
/// let three = NonZeroI32::new(3).unwrap();
/// let min = NonZeroI32::new(std::i32::MIN).unwrap();
///
/// assert_eq!(div(three, two), Some(one));
/// assert_eq!(div(two, three), None);
/// assert_eq!(div(min, minus_one), None);
/// ```
#[macro_export]
macro_rules! assume {
    ($condition: expr) => {
        if !($condition) {
            $crate::unreachable!()
        }
    };
}

/// Hint that the condition is likely to be true.
///
/// The compiler can optimize `if` branchs with
/// this information.
///
/// # Example
///
/// ```
/// #![feature(core_intrinsics)]
/// #![feature(const_likely)]
/// #![feature(const_ptr_is_null)]
/// use std::ptr::{NonNull, null_mut};
///
/// // like NonNull::new()
/// const fn new_nonnull<T: ?Sized>(ptr: *mut T) -> Option<NonNull<T>> {
///     // most of the time, this is used for non null pointers
///     if hint::likely!(!ptr.is_null()) {
///         // SAFETY: checked as non null
///         Some(unsafe { NonNull::new_unchecked(ptr) })
///     } else {
///         None
///     }
/// }
///
/// let mut num = 2;
/// let mut text = String::from("string");
///
/// let num_ptr = &mut num as *mut _;
/// assert_eq!(new_nonnull(num_ptr), NonNull::new(num_ptr));
///
/// let text_ptr = text.as_mut_str() as *mut _;
/// assert_eq!(new_nonnull(text_ptr), NonNull::new(text_ptr));
///
/// assert_eq!(new_nonnull(null_mut::<()>()), None);
/// ```
#[macro_export]
macro_rules! likely {
    ($condition: expr) => {
        std::intrinsics::likely($condition)
    };
}

/// Hint that the condition is likely to be true.
///
/// Negative version of [`hint::likely`](likely).
#[macro_export]
macro_rules! unlikely {
    ($condition: expr) => {
        std::intrinsics::unlikely($condition)
    };
}

#[cfg(test)]
mod tests {
    #[test]
    fn boolen_identity() {
        assert!(crate::likely!(true));
        assert!(!crate::likely!(false));
        assert!(crate::unlikely!(true));
        assert!(!crate::unlikely!(false));
    }

    #[test]
    fn assuming_invariants() {
        use std::ptr::{null_mut, NonNull};

        let ptr = &mut 2 as *mut i32;

        unsafe { crate::assume!(!ptr.is_null()) }
        let nonnull = NonNull::new(ptr);
        unsafe { crate::assume!(nonnull.is_some()) }

        match nonnull {
            None => panic!("impossible"),
            Some(nonnull) => {
                unsafe { crate::assume!(!nonnull.as_ptr().is_null()) };
                unsafe { crate::assume!(nonnull.as_ptr() == ptr) };

                assert_ne!(nonnull.as_ptr(), null_mut());
                assert_eq!(nonnull.as_ptr(), ptr)
            }
        }
    }

    #[test]
    fn const_context() {
        use std::num::NonZeroU32;

        const fn sub2(num: u32) -> Option<u32> {
            let sub = u32::wrapping_sub;

            if crate::likely!(num > 0) {
                let num = sub(num, 1);

                if crate::unlikely!(num == 0) {
                    None
                } else {
                    Some(sub(num, 1))
                }
            } else {
                None
            }
        }

        const fn sub2n(num: NonZeroU32) -> Option<u32> {
            let num = num.get();
            unsafe { crate::assume!(num > 0) };

            sub2(num)
        }

        fn test(num: u32, ans: Option<u32>) {
            let nonzero = NonZeroU32::new(num);

            assert_eq!(sub2(num), nonzero.and_then(sub2n));
            assert_eq!(sub2(num), ans)
        }

        for i in 0..=5 {
            test(i, if i > 1 { Some(i - 2) } else { None })
        }
    }
}
