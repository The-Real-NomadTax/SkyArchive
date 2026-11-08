# SkyArchive

A Rust application for managing a Skylander collection, with optional Portal of Power support.

## Quick start

```bash
cargo run
```

Portal reads need the `usb` feature. That feature uses the pure-Rust `rawusb` crate, so it does not need libusb or a C compiler.

```bash
cargo run --features usb --release
```

`make`, `make run`, `make test`, and `make usb` call the same Cargo commands.

## Menu

1. Add Skylander
2. View Collection
3. Search Skylander
4. Remove Skylander
5. Save Collection (`collection.txt`)
6. Load Collection
7. Connect Portal of Power
8. Read Skylander from Portal
9. Portal Status
10. Search Skylander Database
11. Database Statistics
0. Exit

## Collection file

```
<number of skylanders>
<name>|<element>|<type>|<game>|<year>|<owned>|<description>
```

`owned` is `1` or `0`. This is the same layout the earlier C program wrote. A description may contain `|`.

## Figure database

On startup the program loads the first `skylanders.db.sql` it finds in the working directory, the parent directory, or next to the executable. Rows look like:

```sql
INSERT INTO skylanders (name, element, debut_game, year, type) VALUES ('Eruptor', 'Fire', 'Spyro''s Adventure', 2011, 'Core');
```

The dump has no NFC ids. Rows are numbered from `0xB7` so a portal report in that shape can still name the first row. Doubled quotes (`''`) are one apostrophe.

Names such as `Core`, `Giant`, `Trap Master`, `SuperCharger`, and `Sensei` map onto the figure types stored in collection files (`Spyro`, `Giants`, `Trap Team`, `SuperChargers`, `Imaginators`).

## Portal

Compatible portals are the PS3, PS4, Wii, Wii U, and PC Portal of Power (USB ids `1430:0150` and `1430:0151`). The Xbox portal is not supported. On Linux the portal's kernel driver is detached from interface 0 when the device is claimed. On Windows the portal must be bound to WinUSB. Reading may require administrator rights.

A report is decoded as a command byte followed by a little-endian figure id in bytes 5 through 8.

## Layout

- `src/main.rs` — process entry
- `src/cli.rs` — menu
- `src/skylander.rs` — element, type, and figure
- `src/collection.rs` — collection and `collection.txt`
- `src/database.rs` — SQL dump
- `src/portal.rs` — portal reports and USB

## Tests

```bash
cargo test
```
