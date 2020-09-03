mod field_tuple;

use std::alloc::handle_alloc_error;
use field_tuple::{FieldTuple, layout_with_last_field};
use super::layout::{Layout, Result};
use super::{grow, shrink, NonNull};

type Start<T> = <<T as ReprC>::Fields as FieldTuple>::Start;
type Last<T> = <<T as ReprC>::Fields as FieldTuple>::Last;

pub unsafe trait ReprC {
    type Fields: FieldTuple + ?Sized;

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
            let new_ptr = match unsafe { shrink(NonNull::new_unchecked(self_ptr), self_layout, last_layout) } {
                Err(_) => handle_alloc_error(last_layout.inner()),
                Ok(ptr) => ptr
            };
            last_ptr = unsafe { new_ptr.cast_unsized().as_ptr() }
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
