use mem::alloc::ReprC;
use mem::ptr::NonNull;
use mem::ReprC;

use std::hash::{Hash, Hasher};

type Next<T> = Option<Box<Node<T>>>;

#[repr(C)]
#[derive(Debug, Clone, ReprC)]
pub(super) struct Node<T: ?Sized> {
    pub next: Next<T>,
    pub data: T
}

impl<T: ?Sized> Node<T> {
    #[inline]
    pub fn build(data: Box<T>, next: Next<T>) -> Box<Self> {
        ReprC::expand(data, (next,))
    }

    #[inline]
    pub fn split(self: Box<Self>) -> (Box<T>, Next<T>) {
        let (data, (next,)) = ReprC::split(self, true);
        (data, next)
    }

    #[inline]
    pub const fn as_ptr(&self) -> NonNull<Self> {
        NonNull::from(self)
    }

    #[inline]
    pub unsafe fn push_next_unchecked(&mut self, next: Box<Self>) {
        debug_assert!(self.next.is_none());
        // SAFETY: write is valid, since we take it by reference
        // but caller must guarantee that drop is not necessary
        unsafe { std::ptr::write(&mut self.next, Some(next)) }
    }
}

impl<T: ?Sized + Hash> Hash for Node<T> {
    #[inline]
    fn hash<H: Hasher>(&self, state: &mut H) {
        self.data.hash(state);

        // iteratively hash everything
        let mut next = &self.next;
        while let Some(node) = next {
            node.data.hash(state);

            next = &node.next
        }
    }
}

impl<U, T: ?Sized + PartialEq<U>> PartialEq<Node<U>> for Node<T> {
    #[inline]
    fn eq(&self, other: &Node<U>) -> bool {
        todo!()
    }
}
