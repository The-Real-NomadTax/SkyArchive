# SkyArchive

A C application for managing your Skylander collection with Portal of Power integration!

## Quick Start

```bash
# Build the project
make

# Run the application
./skyarchive
```

## Project Structure

- `src/` - Source code directory
  - `main.c` - Main application entry point
  - `skylander.h/c` - Skylander data structures
  - `collection.h/c` - Collection management
  - `portal.h/c` - Portal of Power USB communication
  - `Makefile` - Build configuration
  - `README.md` - Detailed documentation
  - `.gitignore` - Git ignore rules
- `skyarchive.exe` - Compiled executable (Windows)
- `Makefile` - Root Makefile (delegates to src/)

## Features

- **Collection Management**: Add, remove, search, and view Skylanders
- **Portal Integration**: Read Skylanders directly from Portal of Power
- **File I/O**: Save and load collections
- **Cross-platform**: Works on Windows, Linux, and macOS

## Documentation

For detailed documentation, build instructions, and usage information, see `src/README.md`.

## Building

The build system automatically detects your platform and available libraries:

- **With libusb**: Full Portal of Power functionality
- **Without libusb**: Core features only (portal disabled)

## Portal Compatibility

- ✅ PS3/PS4 Portal of Power
- ✅ Wii/Wii U Portal of Power  
- ✅ PC Portal of Power
- ❌ Xbox Portal (platform-specific)

---

*Happy collecting!* 🎮✨
