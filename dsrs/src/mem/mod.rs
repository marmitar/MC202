//! Memory management utilities

mod nonnull;
pub use nonnull::NonNull;

#[macro_use]
mod layout;
pub use layout::Layout;

// mod alloc;
