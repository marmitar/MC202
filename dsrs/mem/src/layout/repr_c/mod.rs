mod field_tuple;

use super::{Layout, Result};
use field_tuple::FieldTuple;

pub unsafe trait ReprC {
    type Fields: FieldTuple;
}
