#ifndef SKYLANDER_H
#define SKYLANDER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

// Maximum string lengths
#define MAX_NAME_LENGTH 50
#define MAX_ELEMENT_LENGTH 20
#define MAX_GAME_LENGTH 30
#define MAX_TYPE_LENGTH 20

// Skylander element types
typedef enum {
    FIRE,
    WATER,
    AIR,
    EARTH,
    LIFE,
    UNDEAD,
    TECH,
    MAGIC,
    LIGHT,
    DARK,
    KAOS,
    UNKNOWN_ELEMENT
} Element;

// Skylander types
typedef enum {
    GIANTS,
    SWAP_FORCE,
    TRAP_TEAM,
    SUPERCHARGERS,
    IMAGINATORS,
    SPYRO,
    MINI,
    VILLAIN,
    SENSEI_VILLAIN,
    UNKNOWN_TYPE
} SkylanderType;

// Main Skylander structure
typedef struct {
    char name[MAX_NAME_LENGTH];
    Element element;
    SkylanderType type;
    char game[MAX_GAME_LENGTH];
    int year;
    char description[200];
    int owned;
} Skylander;

// Function prototypes
Skylander* create_skylander(const char* name, Element element, SkylanderType type, 
                           const char* game, int year, const char* description);
void free_skylander(Skylander* skylander);
void display_skylander(const Skylander* skylander);
void display_skylander_brief(const Skylander* skylander);

// Utility functions
const char* element_to_string(Element element);
const char* type_to_string(SkylanderType type);
Element string_to_element(const char* str);
SkylanderType string_to_type(const char* str);

#endif // SKYLANDER_H
