#ifndef COLLECTION_H
#define COLLECTION_H

#include "skylander.h"

#define MAX_COLLECTION_SIZE 1000

// Collection structure
typedef struct {
    Skylander** skylanders;
    int count;
    int capacity;
} Collection;

// Collection management functions
Collection* create_collection(void);
void free_collection(Collection* collection);
int add_skylander(Collection* collection, Skylander* skylander);
int remove_skylander(Collection* collection, const char* name);
Skylander* find_skylander(Collection* collection, const char* name);
void display_collection(Collection* collection);

// Interactive functions
void add_skylander_interactive(Collection* collection);
void search_skylander_interactive(Collection* collection);
void remove_skylander_interactive(Collection* collection);

// File I/O functions
int save_collection_to_file(Collection* collection, const char* filename);
int load_collection_from_file(Collection* collection, const char* filename);

// Utility functions
void sort_collection_by_name(Collection* collection);
void sort_collection_by_element(Collection* collection);
int get_collection_stats(Collection* collection, int* total, int* owned);

#endif // COLLECTION_H
