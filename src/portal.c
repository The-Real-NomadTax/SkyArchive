#include "portal.h"
#include "skylander_database.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

#ifdef HAVE_LIBUSB
#include <libusb.h>
#else
// Mock libusb functions for systems without libusb
#define LIBUSB_SUCCESS 0
#define LIBUSB_ERROR_TIMEOUT -7
#define LIBUSB_LOG_LEVEL_DEBUG 4
#define LIBUSB_LOG_LEVEL_NONE 0

typedef struct libusb_context libusb_context;
typedef struct libusb_device libusb_device;
typedef struct libusb_device_handle libusb_device_handle;

typedef struct {
    unsigned char bLength;
    unsigned char bDescriptorType;
    unsigned short bcdUSB;
    unsigned char bDeviceClass;
    unsigned char bDeviceSubClass;
    unsigned char bDeviceProtocol;
    unsigned char bMaxPacketSize0;
    unsigned short idVendor;
    unsigned short idProduct;
    unsigned short bcdDevice;
    unsigned char iManufacturer;
    unsigned char iProduct;
    unsigned char iSerialNumber;
    unsigned char bNumConfigurations;
} libusb_device_descriptor;

static inline int libusb_init(libusb_context** context) {
    printf("Warning: libusb not available. Portal functionality disabled.\n");
    return -1;
}

static inline void libusb_exit(libusb_context* context) { }
static inline void libusb_set_debug(libusb_context* context, int level) { }
static inline ssize_t libusb_get_device_list(libusb_context* context, libusb_device*** list) { return 0; }
static inline void libusb_free_device_list(libusb_device** list, int unref_devices) { }
static inline int libusb_get_device_descriptor(libusb_device* device, libusb_device_descriptor* desc) { return -1; }
static inline int libusb_open(libusb_device* device, libusb_device_handle** handle) { return -1; }
static inline void libusb_close(libusb_device_handle* handle) { }
static inline int libusb_claim_interface(libusb_device_handle* handle, int interface_number) { return -1; }
static inline int libusb_release_interface(libusb_device_handle* handle, int interface_number) { return -1; }
static inline int libusb_interrupt_transfer(libusb_device_handle* handle, unsigned char endpoint, unsigned char* data, int length, int* actual_length, unsigned int timeout) { return -1; }
static inline const char* libusb_error_name(int error_code) { return "libusb not available"; }

#endif

// Global libusb context
static libusb_context* g_usb_context = NULL;

Portal* portal_init(void) {
    Portal* portal = malloc(sizeof(Portal));
    if (!portal) {
        return NULL;
    }
    
    portal->device_handle = NULL;
    portal->state = PORTAL_DISCONNECTED;
    portal->vendor_id = PORTAL_VENDOR_ID;
    portal->product_id = 0; // Will be set during connection
    portal->device_path[0] = '\0';
    
    // Initialize libusb if not already done
    if (!g_usb_context) {
        int result = libusb_init(&g_usb_context);
        if (result < 0) {
            printf("Error: Failed to initialize libusb: %s\n", libusb_error_name(result));
            free(portal);
            return NULL;
        }
        
        // Set debug level for development
        #ifdef DEBUG
        libusb_set_debug(g_usb_context, LIBUSB_LOG_LEVEL_DEBUG);
        #endif
    }
    
    return portal;
}

void portal_cleanup(Portal* portal) {
    if (portal) {
        portal_disconnect(portal);
        free(portal);
    }
    
    // Cleanup libusb context when last portal is destroyed
    if (g_usb_context) {
        libusb_exit(g_usb_context);
        g_usb_context = NULL;
    }
}

int portal_connect(Portal* portal) {
    if (!portal) {
        return 0;
    }
    
    // Try to find and connect to Portal of Power
    libusb_device** devices;
    ssize_t device_count = libusb_get_device_list(g_usb_context, &devices);
    
    if (device_count < 0) {
        printf("Error: Failed to get device list: %s\n", libusb_error_name(device_count));
        return 0;
    }
    
    libusb_device* portal_device = NULL;
    
    // Search for Portal of Power devices
    for (ssize_t i = 0; i < device_count; i++) {
        libusb_device* device = devices[i];
        struct libusb_device_descriptor desc;
        
        int result = libusb_get_device_descriptor(device, &desc);
        if (result < 0) {
            continue;
        }
        
        // Check if this is a Portal of Power
        if (desc.idVendor == PORTAL_VENDOR_ID && 
            (desc.idProduct == PORTAL_PRODUCT_ID || desc.idProduct == PORTAL_ALT_PRODUCT_ID)) {
            portal_device = device;
            portal->product_id = desc.idProduct;
            break;
        }
    }
    
    if (!portal_device) {
        printf("Error: Portal of Power not found. Please ensure it's connected via USB.\n");
        libusb_free_device_list(devices, 1);
        portal->state = PORTAL_DISCONNECTED;
        return 0;
    }
    
    // Open the device
    int result = libusb_open(portal_device, (libusb_device_handle**)&portal->device_handle);
    if (result < 0) {
        printf("Error: Failed to open Portal of Power: %s\n", libusb_error_name(result));
        libusb_free_device_list(devices, 1);
        portal->state = PORTAL_DISCONNECTED;
        return 0;
    }
    
    // Claim the interface (Portal typically uses interface 0)
    result = libusb_claim_interface((libusb_device_handle*)portal->device_handle, 0);
    if (result < 0) {
        printf("Error: Failed to claim Portal interface: %s\n", libusb_error_name(result));
        libusb_close((libusb_device_handle*)portal->device_handle);
        portal->device_handle = NULL;
        libusb_free_device_list(devices, 1);
        portal->state = PORTAL_DISCONNECTED;
        return 0;
    }
    
    // Get device path for identification
    snprintf(portal->device_path, sizeof(portal->device_path), 
             "VID:%04X PID:%04X", portal->vendor_id, portal->product_id);
    
    portal->state = PORTAL_CONNECTED;
    
    printf("Successfully connected to Portal of Power! (%s)\n", portal->device_path);
    
    libusb_free_device_list(devices, 1);
    return 1;
}

int portal_disconnect(Portal* portal) {
    if (!portal || !portal->device_handle) {
        return 0;
    }
    
    // Release the interface
    libusb_release_interface((libusb_device_handle*)portal->device_handle, 0);
    
    // Close the device
    libusb_close((libusb_device_handle*)portal->device_handle);
    
    portal->device_handle = NULL;
    portal->state = PORTAL_DISCONNECTED;
    portal->device_path[0] = '\0';
    
    printf("Disconnected from Portal of Power.\n");
    return 1;
}

int portal_is_connected(Portal* portal) {
    return (portal && portal->state == PORTAL_CONNECTED && portal->device_handle);
}

int portal_detect_skylander(Portal* portal) {
    if (!portal_is_connected(portal)) {
        return 0;
    }
    
    unsigned char buffer[PORTAL_BUFFER_SIZE];
    int bytes_transferred = 0;
    
    // Try to read from the portal
    int result = libusb_interrupt_transfer(
        (libusb_device_handle*)portal->device_handle,
        PORTAL_ENDPOINT_IN,
        buffer,
        PORTAL_BUFFER_SIZE,
        &bytes_transferred,
        100  // Shorter timeout for detection
    );
    
    if (result == LIBUSB_ERROR_TIMEOUT) {
        // No data available, which is normal when no skylander is present
        return 0;
    } else if (result == LIBUSB_ERROR_OVERFLOW) {
        // Overflow means data is available but buffer was too small
        // This often indicates a skylander is present
        return 1;
    } else if (result < 0) {
        // Only show error for unexpected errors, not overflow
        if (result != LIBUSB_ERROR_OVERFLOW) {
            printf("Error reading from portal: %s\n", libusb_error_name(result));
        }
        return -1;
    }
    
    // Check if we received meaningful data (skylander detected)
    // The portal typically sends specific data patterns when a skylander is present
    if (bytes_transferred > 0 && buffer[0] != 0x00) {
        return 1; // Skylander detected
    }
    
    return 0; // No skylander
}

int portal_read_skylander(Portal* portal, Skylander* skylander) {
    if (!portal_is_connected(portal) || !skylander) {
        return 0;
    }
    
    unsigned char buffer[PORTAL_BUFFER_SIZE];
    int bytes_transferred = 0;
    
    // Read data from portal with larger buffer for overflow handling
    int result = libusb_interrupt_transfer(
        (libusb_device_handle*)portal->device_handle,
        PORTAL_ENDPOINT_IN,
        buffer,
        PORTAL_BUFFER_SIZE,
        &bytes_transferred,
        PORTAL_TIMEOUT
    );
    
    if (result == LIBUSB_ERROR_OVERFLOW) {
        // Overflow means we got data but buffer was too small
        // Try to read again with a larger buffer
        unsigned char large_buffer[256];
        int large_bytes_transferred = 0;
        
        result = libusb_interrupt_transfer(
            (libusb_device_handle*)portal->device_handle,
            PORTAL_ENDPOINT_IN,
            large_buffer,
            256,
            &large_bytes_transferred,
            PORTAL_TIMEOUT
        );
        
        if (result < 0) {
            printf("Error reading skylander data after overflow: %s\n", libusb_error_name(result));
            return 0;
        }
        
        bytes_transferred = large_bytes_transferred;
        memcpy(buffer, large_buffer, bytes_transferred < PORTAL_BUFFER_SIZE ? bytes_transferred : PORTAL_BUFFER_SIZE);
    } else if (result < 0) {
        printf("Error reading skylander data: %s\n", libusb_error_name(result));
        return 0;
    }
    
    if (bytes_transferred < 4) {
        printf("Error: Insufficient data received from portal (%d bytes)\n", bytes_transferred);
        return 0;
    }
    
    // Parse skylander data from buffer using correct NFC protocol
    // Based on real data: 53 10 00 00 00 B7 00 00 00 00 00 00 00 00 00 00
    // The 53 is likely a command, real ID appears to be in bytes 5-8: 00 B7 00 00
    unsigned int skylander_id = 0;
    
    // Check if this is actually a valid Skylander response
    // If all bytes are 0x00, it's likely no Skylander
    int all_zero = 1;
    for (int i = 0; i < bytes_transferred; i++) {
        if (buffer[i] != 0x00) {
            all_zero = 0;
            break;
        }
    }
    
    if (all_zero) {
        printf("Debug: No Skylander detected (all zeros)\n");
        return 0;
    }
    
    if (bytes_transferred >= 8) {
        // Try parsing from bytes 5-8 (little endian)
        skylander_id = buffer[5] | (buffer[6] << 8) | (buffer[7] << 16) | (buffer[8] << 24);
    } else if (bytes_transferred >= 4) {
        // Fallback to first 4 bytes
        skylander_id = (buffer[0] << 24) | (buffer[1] << 16) | (buffer[2] << 8) | buffer[3];
    }
    
    // Debug: Print raw buffer data and parsed ID
    printf("Debug: Raw buffer data: ");
    for (int i = 0; i < bytes_transferred && i < 16; i++) {
        printf("%02X ", buffer[i]);
    }
    printf("\n");
    printf("Debug: Parsed Skylander ID: 0x%08X (%u)\n", skylander_id, skylander_id);
    
    // Check if the ID looks reasonable (not too high or suspicious)
    if (skylander_id == 0 || skylander_id > 0xFFFFFF) {
        printf("Debug: Invalid or suspicious ID, treating as no Skylander\n");
        return 0;
    }
    
    // Get skylander information by ID from database
    const char* name = get_skylander_name_by_id(skylander_id);
    Element element = get_skylander_element_by_id(skylander_id);
    SkylanderType type = get_skylander_type_by_id(skylander_id);
    const char* game = get_skylander_game_by_id(skylander_id);
    int year = get_skylander_year_by_id(skylander_id);
    const char* description = get_skylander_description_by_id(skylander_id);
    
    if (!name || strcmp(name, "Unknown") == 0) {
        printf("Unknown Skylander detected (ID: 0x%08X)\n", skylander_id);
        snprintf(skylander->name, MAX_NAME_LENGTH, "Unknown_%08X", skylander_id);
        skylander->element = UNKNOWN_ELEMENT;
        skylander->type = UNKNOWN_TYPE;
        strcpy(skylander->game, game);
        strcpy(skylander->description, description);
        skylander->year = year;
        skylander->owned = 1; // Assume owned if detected on portal
        return 1;
    }
    
    // Fill in the skylander data
    strncpy(skylander->name, name, MAX_NAME_LENGTH - 1);
    skylander->name[MAX_NAME_LENGTH - 1] = '\0';
    skylander->element = element;
    skylander->type = type;
    skylander->owned = 1; // Detected on portal means owned
    skylander->year = year;
    
    // Set game and description from database
    strcpy(skylander->game, game);
    strcpy(skylander->description, description);
    
    printf("Successfully read Skylander: %s\n", name);
    return 1;
}

int portal_wait_for_skylander(Portal* portal, Skylander* skylander, int timeout_ms) {
    if (!portal_is_connected(portal)) {
        return 0;
    }
    
    printf("Waiting for Skylander to be placed on portal...\n");
    printf("(Timeout: %d seconds)\n", timeout_ms / 1000);
    
    int attempts = 0;
    int max_attempts = timeout_ms / 100; // Check every 100ms
    
    while (attempts < max_attempts) {
        int detected = portal_detect_skylander(portal);
        
        if (detected == 1) {
            printf("Skylander detected! Reading data...\n");
            return portal_read_skylander(portal, skylander);
        } else if (detected == -1) {
            printf("Error detecting skylander\n");
            return 0;
        }
        
        usleep(100000); // Wait 100ms
        attempts++;
        
        // Show progress every second
        if (attempts % 10 == 0) {
            printf("Still waiting... (%d seconds remaining)\n", 
                   (timeout_ms - attempts * 100) / 1000);
        }
    }
    
    printf("Timeout: No Skylander detected within %d seconds\n", timeout_ms / 1000);
    return 0;
}

const char* portal_state_to_string(PortalState state) {
    switch (state) {
        case PORTAL_DISCONNECTED: return "Disconnected";
        case PORTAL_CONNECTED: return "Connected";
        case PORTAL_ERROR: return "Error";
        default: return "Unknown";
    }
}

int portal_get_device_info(Portal* portal, char* info_buffer, size_t buffer_size) {
    if (!portal || !info_buffer || buffer_size == 0) {
        return 0;
    }
    
    snprintf(info_buffer, buffer_size, 
             "Portal State: %s\nDevice: %s\nVendor ID: 0x%04X\nProduct ID: 0x%04X",
             portal_state_to_string(portal->state),
             portal->device_path,
             portal->vendor_id,
             portal->product_id);
    
    return 1;
}

void portal_print_device_list(void) {
    if (!g_usb_context) {
        printf("Error: USB context not initialized\n");
        return;
    }
    
    libusb_device** devices;
    ssize_t device_count = libusb_get_device_list(g_usb_context, &devices);
    
    if (device_count < 0) {
        printf("Error: Failed to get device list: %s\n", libusb_error_name(device_count));
        return;
    }
    
    printf("\n=== Connected USB Devices ===\n");
    printf("%-8s %-8s %-20s %-30s\n", "VID", "PID", "Class", "Description");
    printf("------------------------------------------------------------\n");
    
    for (ssize_t i = 0; i < device_count; i++) {
        libusb_device* device = devices[i];
        struct libusb_device_descriptor desc;
        
        int result = libusb_get_device_descriptor(device, &desc);
        if (result < 0) {
            continue;
        }
        
        const char* description = "Unknown Device";
        if (desc.idVendor == PORTAL_VENDOR_ID && 
            (desc.idProduct == PORTAL_PRODUCT_ID || desc.idProduct == PORTAL_ALT_PRODUCT_ID)) {
            description = "Skylanders Portal of Power";
        }
        
        printf("0x%04X  0x%04X  %-20s %-30s\n", 
               desc.idVendor, desc.idProduct, "HID", description);
    }
    
    libusb_free_device_list(devices, 1);
    printf("\n");
}

// Old hardcoded functions removed - now using comprehensive database

// Interactive portal functions
void portal_connect_interactive(Portal* portal) {
    printf("\n=== Connect Portal of Power ===\n");
    
    if (portal_is_connected(portal)) {
        printf("Portal is already connected!\n");
        return;
    }
    
    printf("Searching for Portal of Power...\n");
    portal_print_device_list();
    
    if (portal_connect(portal)) {
        printf("Portal connected successfully!\n");
    } else {
        printf("Failed to connect to Portal of Power.\n");
        printf("Make sure:\n");
        printf("- Portal is connected via USB\n");
        printf("- You have the necessary permissions (try running as administrator/sudo)\n");
        printf("- Portal is compatible (PS3/PS4/Wii/WiiU/PC portals work best)\n");
    }
}

void read_skylander_from_portal_interactive(Portal* portal, Collection* collection) {
    printf("\n=== Read Skylander from Portal ===\n");
    
    if (!portal_is_connected(portal)) {
        printf("Error: Portal not connected! Please connect first (option 7).\n");
        return;
    }
    
    Skylander* skylander = malloc(sizeof(Skylander));
    if (!skylander) {
        printf("Error: Failed to allocate memory for Skylander data.\n");
        return;
    }
    
    printf("Place a Skylander on the Portal of Power...\n");
    
    // Wait for skylander with 30 second timeout
    if (portal_wait_for_skylander(portal, skylander, 30000)) {
        printf("\nSkylander detected and read successfully!\n");
        display_skylander(skylander);
        
        printf("\nWould you like to add this Skylander to your collection? (y/n): ");
        char choice;
        scanf(" %c", &choice);
        
        if (choice == 'y' || choice == 'Y') {
            if (add_skylander(collection, skylander)) {
                printf("Skylander added to collection!\n");
            } else {
                printf("Failed to add Skylander to collection.\n");
                free_skylander(skylander);
            }
        } else {
            free_skylander(skylander);
        }
    } else {
        printf("No Skylander detected or error occurred.\n");
        free_skylander(skylander);
    }
}

void portal_status_interactive(Portal* portal) {
    printf("\n=== Portal Status ===\n");
    
    char info_buffer[512];
    if (portal_get_device_info(portal, info_buffer, sizeof(info_buffer))) {
        printf("%s\n", info_buffer);
    } else {
        printf("Error getting portal information.\n");
    }
    
    if (portal_is_connected(portal)) {
        printf("\nTesting portal communication...\n");
        int detected = portal_detect_skylander(portal);
        
        switch (detected) {
            case 1:
                printf("Status: Portal is working - Skylander detected!\n");
                break;
            case 0:
                printf("Status: Portal is working - No Skylander detected.\n");
                break;
            case -1:
                printf("Status: Portal communication error.\n");
                break;
            default:
                printf("Status: Unknown portal state.\n");
        }
    } else {
        printf("\nPortal is not connected.\n");
    }
}
