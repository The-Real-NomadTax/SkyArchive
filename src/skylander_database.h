#ifndef SKYLANDER_DATABASE_H
#define SKYLANDER_DATABASE_H

#include "skylander.h"

// Database entry structure
typedef struct {
    unsigned int id;
    const char* name;
    Element element;
    SkylanderType type;
    const char* game;
    int year;
    const char* description;
} SkylanderDatabaseEntry;

// Database functions
const SkylanderDatabaseEntry* get_skylander_by_id(unsigned int skylander_id);
const char* get_skylander_name_by_id(unsigned int skylander_id);
Element get_skylander_element_by_id(unsigned int skylander_id);
SkylanderType get_skylander_type_by_id(unsigned int skylander_id);
const char* get_skylander_game_by_id(unsigned int skylander_id);
int get_skylander_year_by_id(unsigned int skylander_id);
const char* get_skylander_description_by_id(unsigned int skylander_id);

// Database statistics
int get_total_skylanders_count(void);
int get_skylanders_by_element_count(Element element);
int get_skylanders_by_type_count(SkylanderType type);

// Search functions
void search_skylanders_by_name(const char* search_term);
void search_skylanders_by_element(Element element);
void search_skylanders_by_type(SkylanderType type);

// Interactive functions
void search_database_interactive(void);
void database_statistics_interactive(void);

#endif // SKYLANDER_DATABASE_H
