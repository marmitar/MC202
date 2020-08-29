/// Tuple with generic types to used to
/// indicate the types on a struct.
/// Automatically implemented up to arity
/// 26.
///
/// # Safety
///
/// This trait should not be implemented
/// for any type. Only tuples.
unsafe trait FieldTuple {
    /// The number of elements in the tuple.
    const ARITY: usize;

    /// The starting tuple
    type Start: FieldTuple + Sized;
    /// The last field on the tuple
    type Last: ?Sized;
}

/// Count identifiers.
macro_rules! count {
    () => { 0 };
    ($head: ident $(, $rest: ident)*) => {
        1 + count!($($rest),*)
    };
}

/// Repeat identifiers, except the last one.
macro_rules! skip_last {
    // stop recursion, ignoring the last item
    ([$last: ident] => [$($type: ident),*]) => {
        ($($type,)*)
    };
    // build accumulator for the last step
    ([$head: ident $(, $rest: ident)+] => [$($type: ident),*]) => {
        skip_last!( [$($rest),+] => [$($type,)* $head] )
    };
}

/// Get the last identifier
macro_rules! last {
    // stop recursion, last identifier
    ($last: ident) => {
        $last
    };
    // continue until last
    ($head: ident $(, $rest: ident)+) => {
        last!($($rest),*)
    };
}

// Implement type list for generic tuple. The
// very last type identifier might be unsized.
//
// `impl_field_tuple(A, B, C)` will impl trait for
// `(A, B, C)` with `<A, B, C: ?Sized>`.
macro_rules! impl_field_tuple {
    // the unit
    () => {
        unsafe impl FieldTuple for () {
            const ARITY: usize = 0;

            type Start = ();
            type Last = !;
        }
    };
    // generic tuple
    ($($type: ident),+) => {
        unsafe impl<$($type),+: ?Sized> FieldTuple for ($($type,)+) {
            const ARITY: usize = count!($($type),+);

            type Start = skip_last!([$($type),+] => []);
            type Last = last!($($type),+);
        }
    };
}

/// Accumulated implementation of generic tuples.
/// Impl trait for the accumulated names, then
/// insert first identifier on the accumulator
/// and repeat.
///
/// `impl_acc{ [C, D] => [A, B] }` will call
/// `impl_field_tuple(A, B)` and
/// `impl_acc{ [D] => [A, B, C] }`
macro_rules! impl_acc {
    // stop recursion, implementing the trait
    // for all identifiers at once
    ([] => [$($type: ident),*]) => {
        impl_field_tuple!($($type),*);
    };
    // recursive step
    ([$head: ident $(, $rest: ident)*] => [$($type: ident),*]) => {
        impl_field_tuple!($($type),*);

        impl_acc!{ [$($rest),*] => [$($type,)* $head] }
    };
}

/// Impl `TypeList` for all tuple arities with
/// given parameter identifiers, in order.
///
/// `impl_all(A, B, C)` will impl for `()`,
/// `(A,)`, `(A, B)` and `(A, B, C)`.
macro_rules! impl_all {
    ($($rest: ident),*) => {
        impl_acc!{ [$($rest),*] => [] }
    };
}

// `FieldTuple` implementation for up to arity 26
impl_all!(
    A, B, C, D, E, F, G, H, I, J, K, L, M,
    N, O, P, Q, R, S, T, U, V, W, X, Y, Z
);
