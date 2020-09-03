//! Memory allocation utilities.
mod layout;
mod repr_c;

pub use layout::{Layout, LayoutErr};
pub use repr_c::{ReprC, Start, Last};
use std::alloc::{Global, AllocRef, AllocErr};
use crate::ptr::NonNull;

/// Grow allocated memory with given allocator.
///
/// On success returns a new pointer to the new block of memory.
/// If `*mut T` is a fat pointer, the metadata is kept.
///
/// When this returns `Ok`, ownership has been moved to the new pointer.
///
/// # Safety
///
/// * `ptr` must denote a block of memory *currently allocated* via `alloc`.
/// * `old_layout` must *fit* that block of memory.
/// * `new_layout.size()` must be greater than or equal to the object's current size.
///
/// See also [`AllocRef::grow`].
///
/// # Errors
///
/// Returns `Err` when, for some reason, the allocation cannot be completed. In this
/// case, ownership is still held by `ptr`.
///
/// More details on [`AllocRef::grow`].
#[inline(always)]
pub unsafe fn grow_with<T: ?Sized, A: AllocRef>(ptr: NonNull<T>, old_layout: Layout, new_layout: Layout, alloc: &mut A) -> Result<NonNull<T>, AllocErr> {
    // SAFETY: caller guarantees that memory was allocated via this allocator and that new size is larger
    // beacuse data at `ptr` is initialized, the old layout fits `inner_ptr`
    let new_ptr = unsafe { alloc.grow(ptr.cast().inner(), old_layout.inner(), new_layout.inner()) }?.cast();

    // change pointer, keep metadata
    Ok(ptr.update(NonNull(new_ptr)))
}

/// Grow allocated memory with [`Global`] allocator.
///
/// Equivalent to `grow_with(ptr, old_layout, new_layout, &mut Global)`.
///
/// # Safety
///
/// See [`grow_with`].
#[inline(always)]
pub unsafe fn grow<T: ?Sized>(ptr: NonNull<T>, old_layout: Layout, new_layout: Layout) -> Result<NonNull<T>, AllocErr> {
    unsafe { grow_with(ptr, old_layout, new_layout, &mut Global) }
}

/// Shrink allocated memory with given allocator.
///
/// On success returns a new pointer to the new block of memory.
/// If `*mut T` is a fat pointer, the metadata is kept.
///
/// When this returns `Ok`, ownership has been moved to the new pointer.
///
/// # Safety
///
/// * `ptr` must denote a block of memory *currently allocated* via `alloc`.
/// * `old_layout` must *fit* that block of memory.
/// * `new_layout.size()` must be smaller than or equal to the object's current size.
///
/// See also [`AllocRef::shrink`].
///
/// # Errors
///
/// Returns `Err` when, for some reason, the allocation cannot be completed. In this
/// case, ownership is still held by `ptr`.
///
/// More details on [`AllocRef::shrink`].
#[inline(always)]
pub unsafe fn shrink_with<T: ?Sized, A: AllocRef>(ptr: NonNull<T>, old_layout: Layout, new_layout: Layout, alloc: &mut A) -> Result<NonNull<T>, AllocErr> {
    // SAFETY: caller guarantees that memory was allocated via this allocator and that new size is larger
    // beacuse data at `ptr` is initialized, the old layout fits `inner_ptr`
    let new_ptr = unsafe { alloc.shrink(ptr.cast().inner(), old_layout.inner(), new_layout.inner()) }?.cast();

    // change pointer, keep metadata
    Ok(ptr.update(NonNull(new_ptr)))
}

/// Shrink allocated memory with [`Global`] allocator.
///
/// Equivalent to `shrink_with(ptr, old_layout, new_layout, &mut Global)`.
///
/// # Safety
///
/// See [`shrink_with`].
#[inline(always)]
pub unsafe fn shrink<T: ?Sized>(ptr: NonNull<T>, old_layout: Layout, new_layout: Layout) -> Result<NonNull<T>, AllocErr> {
    unsafe { shrink_with(ptr, old_layout, new_layout, &mut Global) }
}

