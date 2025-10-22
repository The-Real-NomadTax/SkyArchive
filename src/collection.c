#include "collection.h"

Collection* create_collection(void) {
    Collection* collection = malloc(sizeof(Collection));
    if (!collection) {
        return NULL;
    }
    
    collection->capacity = 100; // Start with capacity for 100 skylanders
    collection->count = 0;
    collection->skylanders = malloc(sizeof(Skylander*) * collection->capacity);
    
    if (!collection->skylanders) {
        free(collection);
        return NULL;
    }
    
    return collection;
}

void free_collection(Collection* collection) {
    if (!collection) return;
    
    if (collection->skylanders) {
        for (int i = 0; i < collection->count; i++) {
            free_skylander(collection->skylanders[i]);
        }
        free(collection->skylanders);
    }
    free(collection);
}

int add_skylander(Collection* collection, Skylander* skylander) {
    if (!collection || !skylander) {
        return 0;
    }
    
    // Check if skylander already exists
    if (find_skylander(collection, skylander->name)) {
        printf("Skylander '%s' already exists in collection!\n", skylander->name);
        return 0;
    }
    
    // Resize if needed
    if (collection->count >= collection->capacity) {
        collection->capacity *= 2;
        Skylander** new_skylanders = realloc(collection->skylanders, 
                                            sizeof(Skylander*) * collection->capacity);
        if (!new_skylanders) {
            return 0;
        }
        collection->skylanders = new_skylanders;
    }
    
    collection->skylanders[collection->count] = skylander;
    collection->count++;
    
    printf("Successfully added '%s' to collection!\n", skylander->name);
    return 1;
}

int remove_skylander(Collection* collection, const char* name) {
    if (!collection || !name) {
        return 0;
    }
    
    for (int i = 0; i < collection->count; i++) {
        if (strcmp(collection->skylanders[i]->name, name) == 0) {
            free_skylander(collection->skylanders[i]);
            
            // Shift remaining elements
            for (int j = i; j < collection->count - 1; j++) {
                collection->skylanders[j] = collection->skylanders[j + 1];
            }
            
            collection->count--;
            printf("Successfully removed '%s' from collection!\n", name);
            return 1;
        }
    }
    
    printf("Skylander '%s' not found in collection!\n", name);
    return 0;
}

Skylander* find_skylander(Collection* collection, const char* name) {
    if (!collection || !name) {
        return NULL;
    }
    
    for (int i = 0; i < collection->count; i++) {
        if (strcmp(collection->skylanders[i]->name, name) == 0) {
            return collection->skylanders[i];
        }
    }
    
    return NULL;
}

void display_collection(Collection* collection) {
    if (!collection) {
        printf("Error: Invalid collection\n");
        return;
    }
    
    if (collection->count == 0) {
        printf("Collection is empty!\n");
        return;
    }
    
    printf("\n=== Your Skylander Collection ===\n");
    printf("Total Skylanders: %d\n\n", collection->count);
    
    printf("%-20s | %-12s | %-15s | %-20s | %s\n", 
           "Name", "Element", "Type", "Game", "Status");
    printf("-----------------------------------------------------------------------------\n");
    
    for (int i = 0; i < collection->count; i++) {
        display_skylander_brief(collection->skylanders[i]);
    }
    
    // Display statistics
    int total, owned;
    get_collection_stats(collection, &total, &owned);
    printf("\nStatistics: %d owned out of %d total (%.1f%%)\n", 
           owned, total, total > 0 ? (float)owned / total * 100 : 0);
}

void add_skylander_interactive(Collection* collection) {
    char name[MAX_NAME_LENGTH];
    char element_str[MAX_ELEMENT_LENGTH];
    char type_str[MAX_TYPE_LENGTH];
    char game[MAX_GAME_LENGTH];
    char description[200];
    int year;
    char owned_input[10];
    
    printf("\n=== Add New Skylander ===\n");
    
    printf("Enter Skylander name: ");
    while (getchar() != '\n'); // Clear input buffer
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = 0; // Remove newline
    
    printf("Enter element (Fire, Water, Air, Earth, Life, Undead, Tech, Magic, Light, Dark): ");
    fgets(element_str, MAX_ELEMENT_LENGTH, stdin);
    element_str[strcspn(element_str, "\n")] = 0;
    
    printf("Enter type (Giants, Swap Force, Trap Team, SuperChargers, Imaginators, Spyro): ");
    fgets(type_str, MAX_TYPE_LENGTH, stdin);
    type_str[strcspn(type_str, "\n")] = 0;
    
    printf("Enter game: ");
    fgets(game, MAX_GAME_LENGTH, stdin);
    game[strcspn(game, "\n")] = 0;
    
    printf("Enter year: ");
    scanf("%d", &year);
    
    printf("Enter description: ");
    while (getchar() != '\n'); // Clear input buffer
    fgets(description, 200, stdin);
    description[strcspn(description, "\n")] = 0;
    
    printf("Do you own this Skylander? (yes/no): ");
    fgets(owned_input, 10, stdin);
    owned_input[strcspn(owned_input, "\n")] = 0;
    
    Element element = string_to_element(element_str);
    SkylanderType type = string_to_type(type_str);
    int owned = (strcmp(owned_input, "yes") == 0 || strcmp(owned_input, "y") == 0);
    
    Skylander* skylander = create_skylander(name, element, type, game, year, description);
    if (skylander) {
        skylander->owned = owned;
        add_skylander(collection, skylander);
    } else {
        printf("Error: Failed to create Skylander!\n");
    }
}

void search_skylander_interactive(Collection* collection) {
    char name[MAX_NAME_LENGTH];
    
    printf("\n=== Search Skylander ===\n");
    printf("Enter Skylander name to search: ");
    while (getchar() != '\n'); // Clear input buffer
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = 0; // Remove newline
    
    Skylander* found = find_skylander(collection, name);
    if (found) {
        display_skylander(found);
    } else {
        printf("Skylander '%s' not found in collection!\n", name);
    }
}

void remove_skylander_interactive(Collection* collection) {
    char name[MAX_NAME_LENGTH];
    
    printf("\n=== Remove Skylander ===\n");
    printf("Enter Skylander name to remove: ");
    while (getchar() != '\n'); // Clear input buffer
    fgets(name, MAX_NAME_LENGTH, stdin);
    name[strcspn(name, "\n")] = 0; // Remove newline
    
    remove_skylander(collection, name);
}

int save_collection_to_file(Collection* collection, const char* filename) {
    if (!collection || !filename) {
        return 0;
    }
    
    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("Error: Could not open file '%s' for writing!\n", filename);
        return 0;
    }
    
    fprintf(file, "%d\n", collection->count);
    
    for (int i = 0; i < collection->count; i++) {
        Skylander* s = collection->skylanders[i];
        fprintf(file, "%s|%s|%s|%s|%d|%d|%s\n",
                s->name,
                element_to_string(s->element),
                type_to_string(s->type),
                s->game,
                s->year,
                s->owned,
                s->description);
    }
    
    fclose(file);
    printf("Collection saved to '%s' successfully!\n", filename);
    return 1;
}

int load_collection_from_file(Collection* collection, const char* filename) {
    if (!collection || !filename) {
        return 0;
    }
    
    FILE* file = fopen(filename, "r");
    if (!file) {
        printf("Error: Could not open file '%s' for reading!\n", filename);
        return 0;
    }
    
    int count;
    if (fscanf(file, "%d", &count) != 1) {
        printf("Error: Invalid file format!\n");
        fclose(file);
        return 0;
    }
    
    // Clear existing collection
    for (int i = 0; i < collection->count; i++) {
        free_skylander(collection->skylanders[i]);
    }
    collection->count = 0;
    
    char line[500];
    while (getchar() != '\n'); // Clear input buffer
    
    for (int i = 0; i < count; i++) {
        if (!fgets(line, sizeof(line), file)) {
            break;
        }
        
        char name[MAX_NAME_LENGTH];
        char element_str[MAX_ELEMENT_LENGTH];
        char type_str[MAX_TYPE_LENGTH];
        char game[MAX_GAME_LENGTH];
        int year, owned;
        char description[200];
        
        if (sscanf(line, "%49[^|]|%19[^|]|%19[^|]|%29[^|]|%d|%d|%199[^\n]",
                   name, element_str, type_str, game, &year, &owned, description) == 7) {
            
            Element element = string_to_element(element_str);
            SkylanderType type = string_to_type(type_str);
            
            Skylander* skylander = create_skylander(name, element, type, game, year, description);
            if (skylander) {
                skylander->owned = owned;
                collection->skylanders[collection->count] = skylander;
                collection->count++;
            }
        }
    }
    
    fclose(file);
    printf("Collection loaded from '%s' successfully! (%d Skylanders)\n", filename, collection->count);
    return 1;
}

void sort_collection_by_name(Collection* collection) {
    if (!collection || collection->count <= 1) return;
    
    for (int i = 0; i < collection->count - 1; i++) {
        for (int j = 0; j < collection->count - i - 1; j++) {
            if (strcmp(collection->skylanders[j]->name, collection->skylanders[j + 1]->name) > 0) {
                Skylander* temp = collection->skylanders[j];
                collection->skylanders[j] = collection->skylanders[j + 1];
                collection->skylanders[j + 1] = temp;
            }
        }
    }
}

int get_collection_stats(Collection* collection, int* total, int* owned) {
    if (!collection || !total || !owned) {
        return 0;
    }
    
    *total = collection->count;
    *owned = 0;
    
    for (int i = 0; i < collection->count; i++) {
        if (collection->skylanders[i]->owned) {
            (*owned)++;
        }
    }
    
    return 1;
}
