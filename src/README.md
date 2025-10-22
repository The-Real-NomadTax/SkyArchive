# SkyArchive

A C application for managing your Skylander collection! Track your Skylanders, organize them by element and type, and keep detailed records of your collection.

## Features

- **Collection Management**: Add, remove, search, and view your Skylander collection
- **Detailed Information**: Store name, element, type, game, year, and descriptions
- **Ownership Tracking**: Mark which Skylanders you own vs. wishlist items
- **File I/O**: Save and load your collection to/from files
- **Interactive Menu**: User-friendly command-line interface
- **Statistics**: View collection statistics and completion percentages
- **Portal of Power Integration**: Connect and read Skylanders directly from the Portal of Power!
- **Automatic Detection**: Automatically identify Skylanders when placed on the portal
- **Real-time Communication**: Live USB communication with Portal of Power hardware

## Skylander Data

Each Skylander in your collection includes:
- **Name**: The character's name
- **Element**: Fire, Water, Air, Earth, Life, Undead, Tech, Magic, Light, or Dark
- **Type**: Giants, Swap Force, Trap Team, SuperChargers, Imaginators, or Spyro
- **Game**: Which Skylanders game they appeared in
- **Year**: Release year
- **Description**: Custom notes or description
- **Ownership Status**: Whether you own the figure or not

## Building

### Prerequisites
- GCC compiler
- Make utility
- **libusb-1.0** (for Portal of Power communication)

### Installing libusb-1.0

**Ubuntu/Debian:**
```bash
sudo apt-get install libusb-1.0-0-dev
```

**Windows:**
- Download libusb from https://libusb.info/
- Extract and add to your system PATH

**macOS:**
```bash
brew install libusb
```

### Compilation
```bash
# Build the project
make

# Or build with debug information
make debug

# Build optimized release version
make release
```

### Running
```bash
# Build and run
make run

# Or run directly
./skyarchive
```

## Usage

When you run SkyArchive, you'll see a main menu with the following options:

1. **Add Skylander** - Add a new Skylander to your collection
2. **View Collection** - Display all Skylanders in your collection
3. **Search Skylander** - Find a specific Skylander by name
4. **Remove Skylander** - Remove a Skylander from your collection
5. **Save Collection** - Save your collection to a file
6. **Load Collection** - Load a previously saved collection
7. **Connect Portal of Power** - Connect to your Skylanders Portal
8. **Read Skylander from Portal** - Automatically read Skylander data from portal
9. **Portal Status** - Check portal connection and test communication

## File Format

Collections are saved in a simple text format with the following structure:
```
<number_of_skylanders>
<name>|<element>|<type>|<game>|<year>|<owned>|<description>
```

## Portal of Power Integration

SkyArchive now supports reading Skylanders directly from the Portal of Power! This feature allows you to:

- **Automatic Detection**: Place a Skylander on the portal and it will be automatically detected
- **Data Reading**: Extract the Skylander's unique ID and identify the character
- **Collection Integration**: Automatically add detected Skylanders to your collection
- **Real-time Communication**: Live USB communication with the portal hardware

### Portal Compatibility

- **Compatible Portals**: PS3, PS4, Wii, Wii U, and PC Portal of Power devices
- **USB Connection**: Connect via USB to your computer
- **Permissions**: On Linux/macOS, you may need to run with `sudo` for USB access
- **Windows**: May require driver installation or running as Administrator

### How to Use Portal Features

1. **Connect Portal**: Use option 7 to connect to your Portal of Power
2. **Place Skylander**: Place any Skylander figure on the portal
3. **Read Data**: Use option 8 to read the Skylander data
4. **Add to Collection**: Choose to add the detected Skylander to your collection

## Project Structure

- `main.c` - Main program entry point and menu system
- `skylander.h/c` - Skylander data structure and utility functions
- `collection.h/c` - Collection management and file I/O
- `portal.h/c` - Portal of Power USB communication
- `Makefile` - Build configuration

## Development

### Adding New Features
The code is structured to be easily extensible. You can add:
- New element types in the `Element` enum
- New Skylander types in the `SkylanderType` enum
- Additional fields to the `Skylander` structure
- New search/filter functionality

### Code Style
- Use consistent indentation (spaces)
- Comment functions and complex logic
- Follow the existing naming conventions
- Use meaningful variable and function names

## License

This project is open source and available under the MIT License.

## Contributing

Feel free to submit issues, feature requests, or pull requests to improve SkyArchive!

---

*Happy collecting!* 🎮✨
