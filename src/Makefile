# SkyArchive Makefile
CC = gcc
CFLAGS = -Wall -Wextra -std=c99 -g
LDFLAGS = 
TARGET = ../skyarchive
SOURCES = main.c skylander.c collection.c portal.c skylander_database.c
OBJECTS = $(SOURCES:.c=.o)
HEADERS = skylander.h collection.h portal.h skylander_database.h

# Check if libusb is available
ifeq ($(OS),Windows_NT)
    # Windows - check for libusb installation
    LIBUSB_PATH := C:/libusb
    LIBUSB_LIB_PATH := C:/libusb/MinGW64/static
    HAVE_LIBUSB := yes
else
    # Unix-like systems
    HAVE_LIBUSB := $(shell pkg-config --exists libusb-1.0 2>/dev/null && echo "yes" || echo "no")
endif

# Default target
all: $(TARGET)

# Build the main executable
$(TARGET): $(OBJECTS)
ifeq ($(HAVE_LIBUSB),yes)
ifeq ($(OS),Windows_NT)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS) -L$(LIBUSB_LIB_PATH) -lusb-1.0
	@echo "Built with libusb support (Windows)"
else
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS) $(shell pkg-config --libs libusb-1.0)
	@echo "Built with libusb support"
endif
else
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Built without libusb support (Portal functionality disabled)"
endif

# Compile source files
%.o: %.c $(HEADERS)
ifeq ($(HAVE_LIBUSB),yes)
ifeq ($(OS),Windows_NT)
	$(CC) $(CFLAGS) -I$(LIBUSB_PATH)/include -DHAVE_LIBUSB -c $< -o $@
else
	$(CC) $(CFLAGS) $(shell pkg-config --cflags libusb-1.0) -DHAVE_LIBUSB -c $< -o $@
endif
else
	$(CC) $(CFLAGS) -c $< -o $@
endif

# Clean build artifacts
clean:
	rm -f $(OBJECTS) $(TARGET) ../skyarchive.exe

# Install (copy to system directory - requires sudo on Linux)
install: $(TARGET)
	sudo cp $(TARGET) /usr/local/bin/

# Uninstall
uninstall:
	sudo rm -f /usr/local/bin/$(TARGET)

# Run the program
run: $(TARGET)
	./$(TARGET)

# Debug build
debug: CFLAGS += -DDEBUG -g3
debug: $(TARGET)

# Release build
release: CFLAGS += -O2 -DNDEBUG
release: clean $(TARGET)

# Help target
help:
	@echo "Available targets:"
	@echo "  all      - Build the project (default)"
	@echo "  clean    - Remove build artifacts"
	@echo "  run      - Build and run the program"
	@echo "  debug    - Build with debug flags"
	@echo "  release  - Build optimized release version"
	@echo "  install  - Install to system (requires sudo)"
	@echo "  uninstall- Remove from system"
	@echo "  help     - Show this help message"
	@echo ""
	@echo "Dependencies:"
	@echo "  libusb-1.0 - For Portal of Power communication"
	@echo "  Install on Ubuntu/Debian: sudo apt-get install libusb-1.0-0-dev"
	@echo "  Install on Windows: Download from https://libusb.info/"
	@echo "  Install on macOS: brew install libusb"

.PHONY: all clean install uninstall run debug release help
