#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "skylander.h"
#include "collection.h"
#include "portal.h"
#include "skylander_database.h"

int main() {
    printf("=== SkyArchive - Skylander Collection Manager ===\n");
    
    Collection* collection = create_collection();
    Portal* portal = portal_init();
    
    int choice;
    do {
        printf("\n=== Main Menu ===\n");
        printf("1. Add Skylander\n");
        printf("2. View Collection\n");
        printf("3. Search Skylander\n");
        printf("4. Remove Skylander\n");
        printf("5. Save Collection\n");
        printf("6. Load Collection\n");
        printf("7. Connect Portal of Power\n");
        printf("8. Read Skylander from Portal\n");
        printf("9. Portal Status\n");
        printf("10. Search Skylander Database\n");
        printf("11. Database Statistics\n");
        printf("0. Exit\n");
        printf("Enter your choice: ");
        
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number.\n");
            while (getchar() != '\n'); // Clear input buffer
            continue;
        }
        
        switch (choice) {
            case 1:
                add_skylander_interactive(collection);
                break;
            case 2:
                display_collection(collection);
                break;
            case 3:
                search_skylander_interactive(collection);
                break;
            case 4:
                remove_skylander_interactive(collection);
                break;
            case 5:
                save_collection_to_file(collection, "collection.txt");
                break;
            case 6:
                load_collection_from_file(collection, "collection.txt");
                break;
            case 7:
                portal_connect_interactive(portal);
                break;
            case 8:
                read_skylander_from_portal_interactive(portal, collection);
                break;
            case 9:
                portal_status_interactive(portal);
                break;
            case 10:
                search_database_interactive();
                break;
            case 11:
                database_statistics_interactive();
                break;
            case 0:
                printf("Thank you for using SkyArchive!\n");
                break;
            default:
                printf("Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
    
    free_collection(collection);
    portal_cleanup(portal);
    return 0;
}
