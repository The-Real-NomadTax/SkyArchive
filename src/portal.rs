//! Portal of Power access.
//!
//! USB transfers are compiled with `--features usb` and go through the
//! pure-Rust `rawusb` crate. Figure reports are decoded either way, so the
//! byte layout can be tested without hardware.

use std::thread;
use std::time::{Duration, Instant};

use crate::database::Database;
use crate::error::Error;
use crate::skylander::Skylander;

#[cfg(feature = "usb")]
use rawusb::ErrorKind;

/// Activision vendor id.
const VENDOR_ID: u16 = 0x1430;
/// PS3/PS4/Wii/PC portal, then the alternate product id.
#[cfg(feature = "usb")]
const PRODUCT_IDS: [u16; 2] = [0x0150, 0x0151];
#[cfg(feature = "usb")]
const ENDPOINT_IN: u8 = 0x81;
#[cfg(feature = "usb")]
const READ_TIMEOUT: Duration = Duration::from_secs(5);
const DETECT_TIMEOUT: Duration = Duration::from_millis(100);
#[cfg(feature = "usb")]
const SHORT_BUFFER: usize = 64;
#[cfg(feature = "usb")]
const LONG_BUFFER: usize = 256;

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub enum PortalState {
    Disconnected,
    Connected,
}

impl PortalState {
    pub const fn as_str(self) -> &'static str {
        match self {
            Self::Disconnected => "Disconnected",
            Self::Connected => "Connected",
        }
    }
}

/// What a portal interrupt report means before the database lookup.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
enum PayloadStatus {
    TooShort(usize),
    Absent,
    Suspicious(u32),
    Id(u32),
}

/// Reads the figure id out of a portal interrupt report.
///
/// A full report looks like `53 10 00 00 00 B7 00 00 00 ...`. Byte 0 is a
/// command. The id is little-endian in bytes 5 through 8. Shorter reports
/// that still include byte 7 use a zero high byte. Anything shorter falls
/// back to the first four bytes, big-endian.
fn classify_payload(buf: &[u8]) -> PayloadStatus {
    if buf.len() < 4 {
        return PayloadStatus::TooShort(buf.len());
    }
    if buf.iter().all(|byte| *byte == 0) {
        return PayloadStatus::Absent;
    }
    let id = if buf.len() >= 9 {
        u32::from_le_bytes([buf[5], buf[6], buf[7], buf[8]])
    } else if buf.len() >= 8 {
        u32::from_le_bytes([buf[5], buf[6], buf[7], 0])
    } else {
        u32::from_be_bytes([buf[0], buf[1], buf[2], buf[3]])
    };
    if id == 0 || id > 0x00FF_FFFF {
        PayloadStatus::Suspicious(id)
    } else {
        PayloadStatus::Id(id)
    }
}

fn format_hex(bytes: &[u8]) -> String {
    let mut out = String::new();
    for byte in bytes.iter().take(16) {
        use std::fmt::Write as _;
        let _ = write!(out, "{byte:02X} ");
    }
    out
}

fn interpret(raw: &[u8], database: &Database) -> Result<Skylander, Error> {
    match classify_payload(raw) {
        PayloadStatus::TooShort(len) => {
            let message = format!("Error: Insufficient data received from portal ({len} bytes)");
            println!("{message}");
            Err(Error::message(message))
        }
        PayloadStatus::Absent => {
            println!("Debug: No Skylander detected (all zeros)");
            Err(Error::message("Debug: No Skylander detected (all zeros)"))
        }
        PayloadStatus::Suspicious(id) => {
            println!("Debug: Raw buffer data: {}", format_hex(raw));
            println!("Debug: Parsed Skylander ID: 0x{id:08X} ({id})");
            println!("Debug: Invalid or suspicious ID, treating as no Skylander");
            Err(Error::message(
                "Debug: Invalid or suspicious ID, treating as no Skylander",
            ))
        }
        PayloadStatus::Id(id) => {
            println!("Debug: Raw buffer data: {}", format_hex(raw));
            println!("Debug: Parsed Skylander ID: 0x{id:08X} ({id})");
            let (figure, known) = database.skylander_for_id(id);
            if known {
                println!("Successfully read Skylander: {}", figure.name);
            } else {
                println!("Unknown Skylander detected (ID: 0x{id:08X})");
            }
            Ok(figure)
        }
    }
}

#[cfg(feature = "usb")]
struct UsbState {
    context: rawusb::Context,
    handle: Option<rawusb::DeviceHandle>,
}

/// A Portal of Power session. Dropping it releases the USB interface.
pub struct Portal {
    state: PortalState,
    vendor_id: u16,
    product_id: u16,
    device_path: String,
    #[cfg(feature = "usb")]
    usb: Option<UsbState>,
}

impl Portal {
    pub fn open() -> Self {
        #[cfg(feature = "usb")]
        {
            match rawusb::Context::new() {
                Ok(context) => {
                    return Self {
                        state: PortalState::Disconnected,
                        vendor_id: VENDOR_ID,
                        product_id: 0,
                        device_path: String::new(),
                        usb: Some(UsbState {
                            context,
                            handle: None,
                        }),
                    };
                }
                Err(err) => {
                    println!(
                        "Warning: USB is unavailable ({err}). Portal functionality disabled."
                    );
                }
            }
        }
        #[cfg(not(feature = "usb"))]
        {
            println!("Warning: USB support was not compiled in. Portal functionality disabled.");
        }
        Self::inactive()
    }

    fn inactive() -> Self {
        Self {
            state: PortalState::Disconnected,
            vendor_id: VENDOR_ID,
            product_id: 0,
            device_path: String::new(),
            #[cfg(feature = "usb")]
            usb: None,
        }
    }

    pub fn is_connected(&self) -> bool {
        self.state == PortalState::Connected
    }

    pub fn print_info(&self) {
        println!("Portal State: {}", self.state.as_str());
        println!("Device: {}", self.device_path);
        println!("Vendor ID: 0x{:04X}", self.vendor_id);
        println!("Product ID: 0x{:04X}", self.product_id);
    }

    pub fn print_devices(&self) {
        #[cfg(feature = "usb")]
        if let Some(usb) = &self.usb {
            print_device_list(&usb.context);
            return;
        }
        println!("Error: USB context not initialized");
    }

    pub fn connect(&mut self) -> Result<(), Error> {
        if self.is_connected() {
            return Ok(());
        }
        #[cfg(feature = "usb")]
        if self.usb.is_some() {
            return self.connect_usb();
        }
        Err(Error::message("Error: USB context not initialized"))
    }

    pub fn disconnect(&mut self) {
        if !self.is_connected() {
            return;
        }
        #[cfg(feature = "usb")]
        if let Some(usb) = self.usb.as_mut() {
            if let Some(handle) = usb.handle.take() {
                let _ = handle.release_interface(0);
            }
        }
        self.state = PortalState::Disconnected;
        self.device_path.clear();
        self.product_id = 0;
        println!("Disconnected from Portal of Power.");
    }

    pub fn detect(&self) -> Result<bool, Error> {
        if !self.is_connected() {
            return Ok(false);
        }
        #[cfg(not(feature = "usb"))]
        {
            Ok(false)
        }
        #[cfg(feature = "usb")]
        {
            let mut buf = vec![0u8; SHORT_BUFFER];
            match self.interrupt_in(&mut buf, DETECT_TIMEOUT) {
                Err(err) if err.kind() == ErrorKind::Timeout => Ok(false),
                Err(err) if err.kind() == ErrorKind::Overflow => Ok(true),
                Err(err) => {
                    println!("Error reading from portal: {err}");
                    Err(Error::message(format!("Error reading from portal: {err}")))
                }
                Ok(0) => Ok(false),
                Ok(_) => Ok(buf[0] != 0),
            }
        }
    }

    pub fn read_skylander(&self, database: &Database) -> Result<Skylander, Error> {
        if !self.is_connected() {
            return Err(Error::message("Error: Portal not connected."));
        }
        let bytes = self.read_payload()?;
        interpret(&bytes, database)
    }

    /// Polls until a figure is present, `timeout` elapses, or the portal errors.
    ///
    /// Each attempt waits about 100ms. A fast empty read is padded so a portal
    /// that answers immediately cannot spin the loop.
    pub fn wait_for_skylander(
        &self,
        database: &Database,
        timeout: Duration,
    ) -> Result<Skylander, Error> {
        if !self.is_connected() {
            return Err(Error::message("Error: Portal not connected."));
        }
        println!("Waiting for Skylander to be placed on portal...");
        println!("(Timeout: {} seconds)", timeout.as_secs());

        let started = Instant::now();
        let mut next_report = Duration::from_secs(1);
        while started.elapsed() < timeout {
            let attempt = Instant::now();
            match self.detect() {
                Ok(true) => {
                    println!("Skylander detected! Reading data...");
                    return self.read_skylander(database);
                }
                Ok(false) => {}
                Err(err) => {
                    println!("Error detecting skylander");
                    return Err(err);
                }
            }
            if let Some(rest) = DETECT_TIMEOUT.checked_sub(attempt.elapsed()) {
                thread::sleep(rest);
            }
            let elapsed = started.elapsed();
            if elapsed >= next_report && elapsed < timeout {
                let remaining = timeout.saturating_sub(elapsed).as_secs();
                if remaining > 0 {
                    println!("Still waiting... ({remaining} seconds remaining)");
                }
                next_report += Duration::from_secs(1);
            }
        }
        let message = format!(
            "Timeout: No Skylander detected within {} seconds",
            timeout.as_secs()
        );
        println!("{message}");
        Err(Error::message(message))
    }

    #[cfg(feature = "usb")]
    fn interrupt_in(&self, buf: &mut [u8], timeout: Duration) -> Result<usize, rawusb::Error> {
        let handle = self
            .usb
            .as_ref()
            .and_then(|usb| usb.handle.as_ref())
            .ok_or_else(|| rawusb::Error::new(ErrorKind::NoDevice))?;
        handle.interrupt_read(ENDPOINT_IN, buf, timeout)
    }

    #[cfg(feature = "usb")]
    fn read_payload(&self) -> Result<Vec<u8>, Error> {
        let mut buf = vec![0u8; SHORT_BUFFER];
        match self.interrupt_in(&mut buf, READ_TIMEOUT) {
            Ok(n) => {
                buf.truncate(n);
                Ok(buf)
            }
            Err(err) if err.kind() == ErrorKind::Overflow => {
                let mut big = vec![0u8; LONG_BUFFER];
                match self.interrupt_in(&mut big, READ_TIMEOUT) {
                    Ok(n) => {
                        big.truncate(n);
                        Ok(big)
                    }
                    Err(err) => {
                        let message = format!("Error reading skylander data after overflow: {err}");
                        println!("{message}");
                        Err(Error::message(message))
                    }
                }
            }
            Err(err) => {
                let message = format!("Error reading skylander data: {err}");
                println!("{message}");
                Err(Error::message(message))
            }
        }
    }

    #[cfg(not(feature = "usb"))]
    fn read_payload(&self) -> Result<Vec<u8>, Error> {
        Err(Error::message("Error: Portal not connected."))
    }

    #[cfg(feature = "usb")]
    fn connect_usb(&mut self) -> Result<(), Error> {
        let context = {
            let Some(usb) = &self.usb else {
                return Err(Error::message("Error: USB context not initialized"));
            };
            usb.context.clone()
        };
        let devices = context.devices().map_err(|err| {
            let message = format!("Error: Failed to get device list: {err}");
            println!("{message}");
            Error::message(message)
        })?;

        let mut opened = None;
        for device in devices {
            if device.vendor_id() == VENDOR_ID && PRODUCT_IDS.contains(&device.product_id()) {
                let product_id = device.product_id();
                match device.open() {
                    Ok(handle) => {
                        opened = Some((handle, product_id));
                        break;
                    }
                    Err(err) => {
                        let message = format!("Error: Failed to open Portal of Power: {err}");
                        println!("{message}");
                        return Err(Error::message(message));
                    }
                }
            }
        }

        let Some((handle, product_id)) = opened else {
            self.state = PortalState::Disconnected;
            let message = "Error: Portal of Power not found. Please ensure it's connected via USB.";
            println!("{message}");
            return Err(Error::message(message));
        };

        if let Err(err) = prepare_portal(&handle) {
            let message = format!("Error: Failed to claim Portal interface: {err}");
            println!("{message}");
            return Err(Error::message(message));
        }

        let Some(usb) = &mut self.usb else {
            return Err(Error::message("Error: USB context not initialized"));
        };
        usb.handle = Some(handle);
        self.product_id = product_id;
        self.vendor_id = VENDOR_ID;
        self.device_path = format!("VID:{VENDOR_ID:04X} PID:{product_id:04X}");
        self.state = PortalState::Connected;
        println!(
            "Successfully connected to Portal of Power! ({})",
            self.device_path
        );
        Ok(())
    }
}

impl Drop for Portal {
    fn drop(&mut self) {
        self.disconnect();
    }
}

#[cfg(feature = "usb")]
fn prepare_portal(handle: &rawusb::DeviceHandle) -> rawusb::Result<()> {
    // Linux often has a kernel HID driver holding interface 0.
    handle.set_auto_detach_kernel_driver(true);
    if matches!(handle.kernel_driver_active(0), Ok(true)) {
        let _ = handle.detach_kernel_driver(0);
    }
    handle.claim_interface(0)
}

#[cfg(feature = "usb")]
fn print_device_list(context: &rawusb::Context) {
    let devices = match context.devices() {
        Ok(devices) => devices,
        Err(err) => {
            println!("Error: Failed to get device list: {err}");
            return;
        }
    };
    println!("\n=== Connected USB Devices ===");
    println!("VID      PID      Class                Description");
    println!("{}", "-".repeat(60));
    for device in devices {
        let description = if device.vendor_id() == VENDOR_ID
            && PRODUCT_IDS.contains(&device.product_id())
        {
            "Skylanders Portal of Power"
        } else {
            "Unknown Device"
        };
        let class = "HID";
        println!(
            "0x{:04X}  0x{:04X}  {class:<20} {description:<30}",
            device.vendor_id(),
            device.product_id()
        );
    }
    println!();
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn reads_the_id_from_the_documented_report() {
        let raw = [
            0x53, 0x10, 0x00, 0x00, 0x00, 0xB7, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00,
            0x00, 0x00,
        ];
        assert_eq!(classify_payload(&raw), PayloadStatus::Id(0xB7));

        let short = [0x53, 0x10, 0x00, 0x00, 0x00, 0xB7, 0x00, 0x00];
        assert_eq!(classify_payload(&short), PayloadStatus::Id(0xB7));
    }

    #[test]
    fn rejects_empty_short_and_suspicious_reports() {
        assert_eq!(classify_payload(&[0, 0, 0]), PayloadStatus::TooShort(3));
        assert_eq!(classify_payload(&[0; 16]), PayloadStatus::Absent);
        assert_eq!(
            classify_payload(&[0x53, 0x10, 0x00, 0x00]),
            PayloadStatus::Suspicious(0x5310_0000)
        );
        let zero_id = [0x53, 0x10, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00, 0x00];
        assert_eq!(classify_payload(&zero_id), PayloadStatus::Suspicious(0));
    }
}
