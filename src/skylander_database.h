#ifndef SKYLANDER_DATABASE_H
#define SKYLANDER_DATABASE_H

#include "skylander.h"

// Database entry structure
typedef struct {
    unsigned int id;
    char name[256];
    Element element;
    SkylanderType type;
    char game[128];
    int year;
    char description[512];
} SkylanderDatabaseEntry;

// Database functions
const SkylanderDatabaseEntry* get_skylander_by_id(unsigned int skylander_id);
const char* get_skylander_name_by_id(unsigned int skylander_id);
Element get_skylander_element_by_id(unsigned int skylander_id);
SkylanderType get_skylander_type_by_id(unsigned int skylander_id);
const char* get_skylander_game_by_id(unsigned int skylander_id);
int get_skylander_year_by_id(unsigned int skylander_id);
const char* get_skylander_description_by_id(unsigned int skylander_id);

// Database management
int load_skylander_database(const char* filename);
void cleanup_skylander_database(void);

// Database statistics
size_t get_total_skylanders_count(void);
size_t get_skylanders_by_element_count(Element element);
size_t get_skylanders_by_type_count(SkylanderType type);

// Search functions
void search_skylanders_by_name(const char* name, SkylanderDatabaseEntry* results, size_t max_results, size_t* result_count);
void search_skylanders_by_element(Element element, SkylanderDatabaseEntry* results, size_t max_results, size_t* result_count);
void search_skylanders_by_type(SkylanderType type, SkylanderDatabaseEntry* results, size_t max_results, size_t* result_count);

// Interactive functions
void search_database_interactive(void);
void database_statistics_interactive(void);

#endif // SKYLANDER_DATABASE_H
