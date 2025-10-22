#ifndef PORTAL_H
#define PORTAL_H

#include "skylander.h"
#include "collection.h"

// Portal of Power USB device identifiers
#define PORTAL_VENDOR_ID    0x1430  // Activision vendor ID
#define PORTAL_PRODUCT_ID   0x0150  // Portal of Power product ID
#define PORTAL_ALT_PRODUCT_ID 0x0151  // Alternative product ID

// USB communication constants
#define PORTAL_ENDPOINT_IN  0x81    // Input endpoint
#define PORTAL_ENDPOINT_OUT 0x01    // Output endpoint
#define PORTAL_TIMEOUT      5000    // 5 second timeout
#define PORTAL_BUFFER_SIZE  64      // Buffer size for USB communication

// Portal states
typedef enum {
    PORTAL_DISCONNECTED,
    PORTAL_CONNECTED,
    PORTAL_ERROR
} PortalState;

// Portal structure
typedef struct {
    void* device_handle;     // libusb device handle
    PortalState state;
    int vendor_id;
    int product_id;
    char device_path[256];   // Device path for identification
} Portal;

// Portal communication functions
Portal* portal_init(void);
void portal_cleanup(Portal* portal);
int portal_connect(Portal* portal);
int portal_disconnect(Portal* portal);
int portal_is_connected(Portal* portal);

// Skylander reading functions
int portal_read_skylander(Portal* portal, Skylander* skylander);
int portal_wait_for_skylander(Portal* portal, Skylander* skylander, int timeout_ms);
int portal_detect_skylander(Portal* portal);

// Utility functions
const char* portal_state_to_string(PortalState state);
int portal_get_device_info(Portal* portal, char* info_buffer, size_t buffer_size);
void portal_print_device_list(void);

// Interactive functions
void portal_connect_interactive(Portal* portal);
void read_skylander_from_portal_interactive(Portal* portal, Collection* collection);
void portal_status_interactive(Portal* portal);

#endif // PORTAL_H
