use super::node::Node;

use std::fmt::{Result, Display, Formatter};

impl<T: ?Sized> Display for Node<T> {
    fn fmt(&self, f: &mut Formatter<'_>) -> Result {
        write!(f, "node")
    }
}
