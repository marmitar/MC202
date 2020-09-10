use super::node::Node;

use mem::ptr::NonNull;

#[derive(Debug)]
pub struct LinkedList<T: ?Sized> {
    pub(super) head: Option<Box<Node<T>>>,
    pub(super) tail: Option<NonNull<Node<T>>>,
    pub(super) length: usize
}

impl<T: ?Sized> LinkedList<T> {
    #[must_use]
    #[inline]
    pub const fn new() -> Self {
        Self { head: None, tail: None, length: 0 }
    }

    #[must_use]
    #[inline]
    pub const fn len(&self) -> usize {
        self.length
    }

    #[must_use]
    #[inline]
    pub const fn is_empty(&self) -> bool {
        self.head.is_none()
    }

    #[must_use]
    #[inline]
    pub const fn tail(&self) -> Option<&T> {
        match self.tail {
            None => None,
            Some(ref ptr) => {
                // SAFETY: ptr is a valid reference
                let tail = unsafe { ptr.as_ref() };
                Some(&tail.data)
            }
        }
    }

    #[must_use]
    #[inline]
    pub const fn tail_mut(&mut self) -> Option<&mut T> {
        match self.tail {
            None => None,
            Some(ref mut ptr) => {
                // SAFETY: 'ptr' is a valid reference and the
                // last node can't be accessed elsewhere
                let tail = unsafe { ptr.as_mut() };
                Some(&mut tail.data)
            }
        }
    }
}

impl<T: ?Sized> LinkedList<T> {
    #[inline]
    unsafe fn push_node_head(&mut self, mut node: Box<Node<T>>) {
        if let Some(next) = self.head.take() {
            // SAFETY: caller must ensure that 'node.next' is
            // currently 'None'
            unsafe { node.push_next_unchecked(next) }

        } else {
            debug_assert!(self.tail.is_none());
            self.tail = Some(node.as_ptr())
        }
        self.head = Some(node);
        self.length += 1
    }

    #[inline]
    unsafe fn push_node_tail(&mut self, node: Box<Node<T>>) {
        if let Some(mut ptr) = self.tail.replace(node.as_ptr()) {
            // SAFETY: 'tail' is a valid mutable reference
            let tail = unsafe { ptr.as_mut() };
            // SAFETY: 'tail.next' is None
            unsafe { tail.push_next_unchecked(node) }

        } else {
            debug_assert!(self.head.is_none());
            self.head = Some(node)
        }
        self.length += 1
    }

    #[inline]
    pub fn push_head_boxed(&mut self, data: Box<T>) {
        // SAFETY: next is None
        unsafe { self.push_node_head(Node::build(data, None)) }
    }

    #[inline]
    pub fn push_tail_boxed(&mut self, data: Box<T>) {
        // SAFETY: next is None
        unsafe { self.push_node_tail(Node::build(data, None)) }
    }

    #[must_use]
    #[inline]
    fn pop_node(&mut self) -> Option<Box<Node<T>>> {
        self.head.take().map(|mut node| {
            self.head = node.next.take();
            if self.head.is_none() {
                self.tail = None
            }
            self.length -= 1;
            node
        })
    }

    #[must_use]
    #[inline]
    pub fn pop_boxed(&mut self) -> Option<Box<T>> {
        self.pop_node().map(|x| x.split().0)
    }
}

impl<T> LinkedList<T> {
    #[inline]
    pub fn push_head(&mut self, data: T) {
        let node = Box::new(Node { next: None, data });
        // SAFETY: next is None
        unsafe { self.push_node_head(node) }
    }

    #[inline]
    pub fn push_tail(&mut self, data: T) {
        let node = Box::new(Node { next: None, data });
        // SAFETY: next is None
        unsafe { self.push_node_tail(node) }
    }

    #[must_use]
    #[inline]
    pub fn pop(&mut self) -> Option<T> {
        self.pop_node().map(|node| node.data)
    }
}

impl<T> Iterator for LinkedList<T> {
    type Item = T;

    #[inline]
    fn next(&mut self) -> Option<T> {
        self.pop()
    }
}
