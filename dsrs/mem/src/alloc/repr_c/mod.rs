mod field_tuple;


use super::layout::{Layout, Result};
use field_tuple::{FieldTuple, layout_with_last_field};

pub type Start<T> = <<T as ReprC>::Fields as FieldTuple>::Start;
pub type Last<T> = <<T as ReprC>::Fields as FieldTuple>::Last;

pub unsafe trait ReprC {
    type Fields: FieldTuple + ?Sized;

    fn layout(last: *const Last<Self>) -> Layout {
        unsafe { layout_with_last_field::<Self::Fields>(last).unwrap().0 }
    }
    fn expand(inner: Box<Last<Self>>, fields: Start<Self>) -> Box<Self>;
    fn split(self: Box<Self>) -> (Box<Last<Self>>, Start<Self>);
}

