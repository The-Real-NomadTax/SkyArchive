#include "skylander.h"

Skylander* create_skylander(const char* name, Element element, SkylanderType type, 
                           const char* game, int year, const char* description) {
    Skylander* skylander = malloc(sizeof(Skylander));
    if (!skylander) {
        return NULL;
    }
    
    strncpy(skylander->name, name, MAX_NAME_LENGTH - 1);
    skylander->name[MAX_NAME_LENGTH - 1] = '\0';
    
    skylander->element = element;
    skylander->type = type;
    skylander->year = year;
    skylander->owned = 0; // Default to not owned
    
    strncpy(skylander->game, game, MAX_GAME_LENGTH - 1);
    skylander->game[MAX_GAME_LENGTH - 1] = '\0';
    
    strncpy(skylander->description, description, 199);
    skylander->description[199] = '\0';
    
    return skylander;
}

void free_skylander(Skylander* skylander) {
    if (skylander) {
        free(skylander);
    }
}

void display_skylander(const Skylander* skylander) {
    if (!skylander) {
        printf("Error: Invalid Skylander data\n");
        return;
    }
    
    printf("\n=== %s ===\n", skylander->name);
    printf("Element: %s\n", element_to_string(skylander->element));
    printf("Type: %s\n", type_to_string(skylander->type));
    printf("Game: %s\n", skylander->game);
    printf("Year: %d\n", skylander->year);
    printf("Status: %s\n", skylander->owned ? "Owned" : "Not Owned");
    printf("Description: %s\n", skylander->description);
    printf("========================\n");
}

void display_skylander_brief(const Skylander* skylander) {
    if (!skylander) {
        printf("Error: Invalid Skylander data\n");
        return;
    }
    
    printf("%-20s | %-12s | %-15s | %-20s | %s\n", 
           skylander->name, 
           element_to_string(skylander->element),
           type_to_string(skylander->type),
           skylander->game,
           skylander->owned ? "Owned" : "Not Owned");
}

const char* element_to_string(Element element) {
    switch (element) {
        case FIRE: return "Fire";
        case WATER: return "Water";
        case AIR: return "Air";
        case EARTH: return "Earth";
        case LIFE: return "Life";
        case UNDEAD: return "Undead";
        case TECH: return "Tech";
        case MAGIC: return "Magic";
        case LIGHT: return "Light";
        case DARK: return "Dark";
        default: return "Unknown";
    }
}

const char* type_to_string(SkylanderType type) {
    switch (type) {
        case GIANTS: return "Giants";
        case SWAP_FORCE: return "Swap Force";
        case TRAP_TEAM: return "Trap Team";
        case SUPERCHARGERS: return "SuperChargers";
        case IMAGINATORS: return "Imaginators";
        case SPYRO: return "Spyro";
        default: return "Unknown";
    }
}

Element string_to_element(const char* str) {
    if (!str) return UNKNOWN_ELEMENT;
    
    if (strcmp(str, "Fire") == 0) return FIRE;
    if (strcmp(str, "Water") == 0) return WATER;
    if (strcmp(str, "Air") == 0) return AIR;
    if (strcmp(str, "Earth") == 0) return EARTH;
    if (strcmp(str, "Life") == 0) return LIFE;
    if (strcmp(str, "Undead") == 0) return UNDEAD;
    if (strcmp(str, "Tech") == 0) return TECH;
    if (strcmp(str, "Magic") == 0) return MAGIC;
    if (strcmp(str, "Light") == 0) return LIGHT;
    if (strcmp(str, "Dark") == 0) return DARK;
    
    return UNKNOWN_ELEMENT;
}

SkylanderType string_to_type(const char* str) {
    if (!str) return UNKNOWN_TYPE;
    
    if (strcmp(str, "Giants") == 0) return GIANTS;
    if (strcmp(str, "Swap Force") == 0) return SWAP_FORCE;
    if (strcmp(str, "Trap Team") == 0) return TRAP_TEAM;
    if (strcmp(str, "SuperChargers") == 0) return SUPERCHARGERS;
    if (strcmp(str, "Imaginators") == 0) return IMAGINATORS;
    if (strcmp(str, "Spyro") == 0) return SPYRO;
    
    return UNKNOWN_TYPE;
}
