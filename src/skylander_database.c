#include "skylander_database.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

// Dynamic Skylander Database loaded from SQL file
static SkylanderDatabaseEntry* skylander_database = NULL;
static size_t database_size = 0;


// Load database from SQL file
int load_skylander_database(const char* filename) {
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open database file '%s'\n", filename);
        return 0;
    }
    
    // Count lines first to allocate memory
    int line_count = 0;
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "INSERT INTO skylanders") != NULL) {
            line_count++;
        }
    }
    
    if (line_count == 0) {
        printf("Error: No Skylander data found in file\n");
        fclose(file);
        return 0;
    }
    
    // Allocate memory for database
    skylander_database = malloc(line_count * sizeof(SkylanderDatabaseEntry));
    if (!skylander_database) {
        printf("Error: Could not allocate memory for database\n");
        fclose(file);
        return 0;
    }
    
    // Reset file pointer and parse data
    rewind(file);
    database_size = 0;
    
    while (fgets(line, sizeof(line), file)) {
        if (strstr(line, "INSERT INTO skylanders") != NULL) {
            // Parse INSERT statement: INSERT INTO skylanders (name, element, debut_game, year, type) VALUES ('Name', 'Element', 'Game', Year, 'Type');
            char name[256], element[64], game[128], type[64];
            int year;
            
            // Extract values from the INSERT statement
            if (sscanf(line, "INSERT INTO skylanders (name, element, debut_game, year, type) VALUES ('%255[^']', '%63[^']', '%127[^']', %d, '%63[^']');", 
                     name, element, game, &year, type) == 5) {
                
                // Generate a simple ID based on position (you can modify this)
                unsigned int id = 0x000000B7 + database_size; // Start with Eruptor's ID
                
                skylander_database[database_size].id = id;
                strncpy(skylander_database[database_size].name, name, sizeof(skylander_database[database_size].name) - 1);
                skylander_database[database_size].name[sizeof(skylander_database[database_size].name) - 1] = '\0';
                skylander_database[database_size].element = string_to_element(element);
                skylander_database[database_size].type = string_to_type(type);
                strncpy(skylander_database[database_size].game, game, sizeof(skylander_database[database_size].game) - 1);
                skylander_database[database_size].game[sizeof(skylander_database[database_size].game) - 1] = '\0';
                skylander_database[database_size].year = year;
                strncpy(skylander_database[database_size].description, name, sizeof(skylander_database[database_size].description) - 1);
                skylander_database[database_size].description[sizeof(skylander_database[database_size].description) - 1] = '\0';
                
                database_size++;
            }
        }
    }
    
    fclose(file);
    printf("Loaded %zu Skylanders from database file\n", database_size);
    return 1;
}

// Get Skylander by ID
const SkylanderDatabaseEntry* get_skylander_by_id(unsigned int id) {
    for (size_t i = 0; i < database_size; i++) {
        if (skylander_database[i].id == id) {
            return &skylander_database[i];
        }
    }
    return NULL;
}

// Get Skylander name by ID
const char* get_skylander_name_by_id(unsigned int id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(id);
    return entry ? entry->name : "Unknown";
}

// Get Skylander element by ID
Element get_skylander_element_by_id(unsigned int id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(id);
    return entry ? entry->element : UNKNOWN_ELEMENT;
}

// Get Skylander type by ID
SkylanderType get_skylander_type_by_id(unsigned int id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(id);
    return entry ? entry->type : UNKNOWN_TYPE;
}

// Get Skylander game by ID
const char* get_skylander_game_by_id(unsigned int id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(id);
    return entry ? entry->game : "Unknown";
}

// Get Skylander year by ID
int get_skylander_year_by_id(unsigned int id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(id);
    return entry ? entry->year : 0;
}

// Get Skylander description by ID
const char* get_skylander_description_by_id(unsigned int id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(id);
    return entry ? entry->description : "Unknown Skylander";
}

// Get total count of Skylanders
size_t get_total_skylanders_count(void) {
    return database_size;
}

// Get count of Skylanders by element
size_t get_skylanders_by_element_count(Element element) {
    size_t count = 0;
    for (size_t i = 0; i < database_size; i++) {
        if (skylander_database[i].element == element) {
            count++;
        }
    }
    return count;
}

// Get count of Skylanders by type
size_t get_skylanders_by_type_count(SkylanderType type) {
    size_t count = 0;
    for (size_t i = 0; i < database_size; i++) {
        if (skylander_database[i].type == type) {
            count++;
        }
    }
    return count;
}

// Search Skylanders by name
void search_skylanders_by_name(const char* name, SkylanderDatabaseEntry* results, size_t max_results, size_t* result_count) {
    *result_count = 0;
    for (size_t i = 0; i < database_size && *result_count < max_results; i++) {
        if (strstr(skylander_database[i].name, name) != NULL) {
            results[*result_count] = skylander_database[i];
            (*result_count)++;
        }
    }
}

// Search Skylanders by element
void search_skylanders_by_element(Element element, SkylanderDatabaseEntry* results, size_t max_results, size_t* result_count) {
    *result_count = 0;
    for (size_t i = 0; i < database_size && *result_count < max_results; i++) {
        if (skylander_database[i].element == element) {
            results[*result_count] = skylander_database[i];
            (*result_count)++;
        }
    }
}

// Search Skylanders by type
void search_skylanders_by_type(SkylanderType type, SkylanderDatabaseEntry* results, size_t max_results, size_t* result_count) {
    *result_count = 0;
    for (size_t i = 0; i < database_size && *result_count < max_results; i++) {
        if (skylander_database[i].type == type) {
            results[*result_count] = skylander_database[i];
            (*result_count)++;
        }
    }
}

// Interactive database search
void search_database_interactive(void) {
    printf("\n=== Search Skylander Database ===\n");
    printf("1. Search by name\n");
    printf("2. Search by element\n");
    printf("3. Search by type\n");
    printf("Enter your choice: ");
    
    int choice;
    scanf("%d", &choice);
    
    SkylanderDatabaseEntry results[100];
    size_t result_count = 0;
    
    switch (choice) {
        case 1: {
            printf("Enter name to search: ");
            char name[256];
            scanf("%s", name);
            search_skylanders_by_name(name, results, 100, &result_count);
            break;
        }
        case 2: {
            printf("Enter element (Air, Earth, Fire, Life, Magic, Tech, Undead, Water): ");
            char element_str[64];
            scanf("%s", element_str);
            Element element = string_to_element(element_str);
            search_skylanders_by_element(element, results, 100, &result_count);
            break;
        }
        case 3: {
            printf("Enter type (Core, Giant, Swap Force, Trap Master, SuperCharger, Sensei, Mini, Villain): ");
            char type_str[64];
            scanf("%s", type_str);
            SkylanderType type = string_to_type(type_str);
            search_skylanders_by_type(type, results, 100, &result_count);
            break;
        }
        default:
            printf("Invalid choice\n");
            return;
    }
    
    if (result_count == 0) {
        printf("No Skylanders found matching your criteria.\n");
    } else {
        printf("\nFound %zu Skylanders:\n", result_count);
        for (size_t i = 0; i < result_count; i++) {
            printf("%zu. %s (%s, %s, %d)\n", i + 1, results[i].name, 
                   results[i].element == FIRE ? "Fire" : 
                   results[i].element == WATER ? "Water" :
                   results[i].element == EARTH ? "Earth" :
                   results[i].element == AIR ? "Air" :
                   results[i].element == LIFE ? "Life" :
                   results[i].element == MAGIC ? "Magic" :
                   results[i].element == TECH ? "Tech" :
                   results[i].element == UNDEAD ? "Undead" : "Unknown",
                   results[i].game, results[i].year);
        }
    }
}

// Interactive database statistics
void database_statistics_interactive(void) {
    printf("\n=== Skylander Database Statistics ===\n");
    printf("Total Skylanders: %zu\n", get_total_skylanders_count());
    printf("\nBy Element:\n");
    printf("Fire: %zu\n", get_skylanders_by_element_count(FIRE));
    printf("Water: %zu\n", get_skylanders_by_element_count(WATER));
    printf("Earth: %zu\n", get_skylanders_by_element_count(EARTH));
    printf("Air: %zu\n", get_skylanders_by_element_count(AIR));
    printf("Life: %zu\n", get_skylanders_by_element_count(LIFE));
    printf("Magic: %zu\n", get_skylanders_by_element_count(MAGIC));
    printf("Tech: %zu\n", get_skylanders_by_element_count(TECH));
    printf("Undead: %zu\n", get_skylanders_by_element_count(UNDEAD));
    printf("Light: %zu\n", get_skylanders_by_element_count(LIGHT));
    printf("Dark: %zu\n", get_skylanders_by_element_count(DARK));
    printf("Kaos: %zu\n", get_skylanders_by_element_count(KAOS));
    
    printf("\nBy Type:\n");
    printf("Core: %zu\n", get_skylanders_by_type_count(SPYRO));
    printf("Giant: %zu\n", get_skylanders_by_type_count(GIANTS));
    printf("Swap Force: %zu\n", get_skylanders_by_type_count(SWAP_FORCE));
    printf("Trap Master: %zu\n", get_skylanders_by_type_count(TRAP_TEAM));
    printf("SuperCharger: %zu\n", get_skylanders_by_type_count(SUPERCHARGERS));
    printf("Sensei: %zu\n", get_skylanders_by_type_count(IMAGINATORS));
    printf("Mini: %zu\n", get_skylanders_by_type_count(MINI));
    printf("Villain: %zu\n", get_skylanders_by_type_count(VILLAIN));
    printf("Sensei Villain: %zu\n", get_skylanders_by_type_count(SENSEI_VILLAIN));
}

// Cleanup database memory
void cleanup_skylander_database(void) {
    if (skylander_database) {
        free(skylander_database);
        skylander_database = NULL;
        database_size = 0;
    }
}