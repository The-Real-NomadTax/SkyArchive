//! SkyArchive keeps a Skylander collection, reads a SQL figure list, and can
//! talk to a Portal of Power when built with `--features usb`.

pub mod cli;
pub mod collection;
pub mod database;
pub mod error;
pub mod portal;
pub mod skylander;
