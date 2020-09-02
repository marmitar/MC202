//! Memory allocation utilities.

mod layout;
mod repr_c;

pub use layout::{Layout, LayoutErr};

use std::alloc::{Global, AllocRef, AllocErr};
use crate::ptr::NonNull;

/// Grow allocated memory with given allocator.
///
/// # Safety
#[inline]
pub unsafe fn grow_with<T: ?Sized, A: AllocRef>(ptr: NonNull<T>, new_layout: Layout, alloc: &mut A) -> Result<NonNull<T>, AllocErr> {
    let new_layout = new_layout.inner();
    let old_layout = unsafe { Layout::for_value_raw::<T>(ptr.as_ptr()) }.inner();
    let inner_ptr = ptr.cast().inner();

    // increase allocated memory
    let new_ptr = unsafe { alloc.grow(inner_ptr, old_layout, new_layout) }?.cast();

    // change pointer, keep metadata
    Ok(ptr.update(NonNull(new_ptr)))
}

/// # Safety
#[inline]
pub unsafe fn grow<T: ?Sized>(ptr: NonNull<T>, new_layout: Layout) -> Result<NonNull<T>, AllocErr> {
    unsafe { grow_with(ptr, new_layout, &mut Global) }
}

/// # Safety
#[inline]
pub unsafe fn shrink_with<T: ?Sized, A: AllocRef>(ptr: NonNull<T>, new_layout: Layout, alloc: &mut A) -> Result<NonNull<T>, AllocErr> {
    let new_layout = new_layout.inner();
    let old_layout = unsafe { Layout::for_value_raw::<T>(ptr.as_ptr()) }.inner();
    let inner_ptr = ptr.cast().inner();

    // reduces allocated memory
    let new_ptr = unsafe { alloc.shrink(inner_ptr, old_layout, new_layout) }?.cast();

    // change pointer, keep metadata
    Ok(ptr.update(NonNull(new_ptr)))
}

/// # Safety
#[inline]
pub unsafe fn shrink<T: ?Sized>(ptr: NonNull<T>, new_layout: Layout) -> Result<NonNull<T>, AllocErr> {
    unsafe { shrink_with(ptr, new_layout, &mut Global) }
}

