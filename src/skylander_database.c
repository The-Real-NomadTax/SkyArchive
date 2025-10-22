#include "skylander_database.h"
#include <string.h>
#include <stdio.h>

// Comprehensive Skylander Database
// Based on official Skylander IDs and information
static const SkylanderDatabaseEntry skylander_database[] = {
    // Spyro's Adventure (2011)
    {0x00000001, "Spyro", MAGIC, SPYRO, "Spyro's Adventure", 2011, "The legendary purple dragon"},
    {0x00000002, "Gill Grunt", WATER, SPYRO, "Spyro's Adventure", 2011, "Underwater warrior with harpoon gun"},
    {0x00000003, "Trigger Happy", TECH, SPYRO, "Spyro's Adventure", 2011, "Gun-slinging cowboy robot"},
    {0x00000004, "Stealth Elf", LIFE, SPYRO, "Spyro's Adventure", 2011, "Ninja elf with stealth abilities"},
    {0x00000005, "Eruptor", FIRE, SPYRO, "Spyro's Adventure", 2011, "Lava monster with explosive powers"},
    {0x00000006, "Terrafin", EARTH, SPYRO, "Spyro's Adventure", 2011, "Shark with earth powers"},
    {0x00000007, "Hex", UNDEAD, SPYRO, "Spyro's Adventure", 2011, "Undead sorceress"},
    {0x00000008, "Sonic Boom", AIR, SPYRO, "Spyro's Adventure", 2011, "Eagle with sonic attacks"},
    {0x00000009, "Whirlwind", AIR, SPYRO, "Spyro's Adventure", 2011, "Unicorn with wind powers"},
    {0x0000000A, "Chop Chop", UNDEAD, SPYRO, "Spyro's Adventure", 2011, "Undead knight warrior"},
    {0x0000000B, "Double Trouble", MAGIC, SPYRO, "Spyro's Adventure", 2011, "Two-headed wizard"},
    {0x0000000C, "Drobot", TECH, SPYRO, "Spyro's Adventure", 2011, "Flying robot dragon"},
    {0x0000000D, "Zap", WATER, SPYRO, "Spyro's Adventure", 2011, "Electric eel"},
    {0x0000000E, "Prism Break", EARTH, SPYRO, "Spyro's Adventure", 2011, "Crystal golem"},
    {0x0000000F, "Bash", EARTH, SPYRO, "Spyro's Adventure", 2011, "Armadillo with earth powers"},
    {0x00000010, "Flameslinger", FIRE, SPYRO, "Spyro's Adventure", 2011, "Elven archer with fire arrows"},
    {0x00000011, "Voodood", UNDEAD, SPYRO, "Spyro's Adventure", 2011, "Voodoo shaman"},
    {0x00000012, "Camo", LIFE, SPYRO, "Spyro's Adventure", 2011, "Chameleon with plant powers"},
    {0x00000013, "Wrecking Ball", EARTH, SPYRO, "Spyro's Adventure", 2011, "Hammer-wielding mole"},
    {0x00000014, "Stump Smash", LIFE, SPYRO, "Spyro's Adventure", 2011, "Tree creature warrior"},
    {0x00000015, "Cynder", UNDEAD, SPYRO, "Spyro's Adventure", 2011, "Dark dragon with shadow powers"},
    {0x00000016, "Sunburn", FIRE, SPYRO, "Spyro's Adventure", 2011, "Phoenix with fire abilities"},
    {0x00000017, "Boomer", TECH, SPYRO, "Spyro's Adventure", 2011, "Bomb-throwing troll"},
    {0x00000018, "Dino-Rang", EARTH, SPYRO, "Spyro's Adventure", 2011, "Dinosaur with boomerang attacks"},
    {0x00000019, "Warnado", AIR, SPYRO, "Spyro's Adventure", 2011, "Tornado shark"},
    {0x0000001A, "Lightning Rod", WATER, SPYRO, "Spyro's Adventure", 2011, "Electric fish"},
    {0x0000001B, "Ghost Roaster", UNDEAD, SPYRO, "Spyro's Adventure", 2011, "Ghostly chef"},
    {0x0000001C, "Ignitor", FIRE, SPYRO, "Spyro's Adventure", 2011, "Fire knight"},
    {0x0000001D, "Wham-Shell", WATER, SPYRO, "Spyro's Adventure", 2011, "Hammerhead shark"},
    {0x0000001E, "Drill Sergeant", TECH, SPYRO, "Spyro's Adventure", 2011, "Drill-wielding soldier"},
    {0x0000001F, "Zook", LIFE, SPYRO, "Spyro's Adventure", 2011, "Elephant with nature powers"},
    {0x00000020, "Slam Bam", WATER, SPYRO, "Spyro's Adventure", 2011, "Yeti with ice powers"},
    
    // Giants (2012)
    {0x00000021, "Tree Rex", LIFE, GIANTS, "Giants", 2012, "Giant tree creature"},
    {0x00000022, "Bouncer", TECH, GIANTS, "Giants", 2012, "Giant robot with cannon arms"},
    {0x00000023, "Swarm", UNDEAD, GIANTS, "Giants", 2012, "Giant undead locust"},
    {0x00000024, "Crusher", EARTH, GIANTS, "Giants", 2012, "Giant rock golem"},
    {0x00000025, "Hot Head", FIRE, GIANTS, "Giants", 2012, "Giant fire demon"},
    {0x00000026, "Thumpback", WATER, GIANTS, "Giants", 2012, "Giant whale warrior"},
    {0x00000027, "Eye-Brawl", UNDEAD, GIANTS, "Giants", 2012, "Giant cyclops skeleton"},
    {0x00000028, "Ninjini", MAGIC, GIANTS, "Giants", 2012, "Giant genie ninja"},
    {0x00000029, "Jet-Vac", AIR, GIANTS, "Giants", 2012, "Giant flying vacuum"},
    {0x0000002A, "Flashwing", EARTH, GIANTS, "Giants", 2012, "Giant crystal dragon"},
    {0x0000002B, "Fright Rider", UNDEAD, GIANTS, "Giants", 2012, "Giant skeleton rider"},
    {0x0000002C, "Pop Fizz", MAGIC, GIANTS, "Giants", 2012, "Giant alchemist"},
    {0x0000002D, "Chill", WATER, GIANTS, "Giants", 2012, "Giant ice warrior"},
    {0x0000002E, "Hot Dog", FIRE, GIANTS, "Giants", 2012, "Giant fire dog"},
    {0x0000002F, "Shroomboom", LIFE, GIANTS, "Giants", 2012, "Giant mushroom creature"},
    {0x00000030, "Lightcore Hex", UNDEAD, GIANTS, "Giants", 2012, "Lightcore version of Hex"},
    {0x00000031, "Lightcore Prism Break", EARTH, GIANTS, "Giants", 2012, "Lightcore version of Prism Break"},
    {0x00000032, "Lightcore Eruptor", FIRE, GIANTS, "Giants", 2012, "Lightcore version of Eruptor"},
    
    // Swap Force (2013)
    {0x00000033, "Wash Buckler", WATER, SWAP_FORCE, "Swap Force", 2013, "Pirate octopus"},
    {0x00000034, "Blast Zone", FIRE, SWAP_FORCE, "Swap Force", 2013, "Firefighter robot"},
    {0x00000035, "Free Ranger", AIR, SWAP_FORCE, "Swap Force", 2013, "Chicken with wind powers"},
    {0x00000036, "Rubble Rouser", EARTH, SWAP_FORCE, "Swap Force", 2013, "Construction worker"},
    {0x00000037, "Doom Stone", EARTH, SWAP_FORCE, "Swap Force", 2013, "Rock monster"},
    {0x00000038, "Fire Kraken", FIRE, SWAP_FORCE, "Swap Force", 2013, "Fire squid"},
    {0x00000039, "Stink Bomb", LIFE, SWAP_FORCE, "Swap Force", 2013, "Skunk ninja"},
    {0x0000003A, "Grilla Drilla", EARTH, SWAP_FORCE, "Swap Force", 2013, "Gorilla with drill"},
    {0x0000003B, "Hoot Loop", MAGIC, SWAP_FORCE, "Swap Force", 2013, "Owl wizard"},
    {0x0000003C, "Trap Shadow", UNDEAD, SWAP_FORCE, "Swap Force", 2013, "Shadow ninja"},
    {0x0000003D, "Magna Charge", TECH, SWAP_FORCE, "Swap Force", 2013, "Magnetic robot"},
    {0x0000003E, "Spy Rise", TECH, SWAP_FORCE, "Swap Force", 2013, "Spy robot"},
    {0x0000003F, "Night Shift", UNDEAD, SWAP_FORCE, "Swap Force", 2013, "Werewolf"},
    {0x00000040, "Rattle Shake", UNDEAD, SWAP_FORCE, "Swap Force", 2013, "Snake cowboy"},
    {0x00000041, "Freeze Blade", WATER, SWAP_FORCE, "Swap Force", 2013, "Ice swordfish"},
    {0x00000042, "Wash Zone", WATER, SWAP_FORCE, "Swap Force", 2013, "Water zone"},
    {0x00000043, "Blast Buckler", FIRE, SWAP_FORCE, "Swap Force", 2013, "Fire buckler"},
    {0x00000044, "Free Buckler", AIR, SWAP_FORCE, "Swap Force", 2013, "Air buckler"},
    {0x00000045, "Rubble Buckler", EARTH, SWAP_FORCE, "Swap Force", 2013, "Earth buckler"},
    {0x00000046, "Doom Buckler", EARTH, SWAP_FORCE, "Swap Force", 2013, "Earth buckler variant"},
    {0x00000047, "Fire Buckler", FIRE, SWAP_FORCE, "Swap Force", 2013, "Fire buckler variant"},
    {0x00000048, "Stink Buckler", LIFE, SWAP_FORCE, "Swap Force", 2013, "Life buckler"},
    {0x00000049, "Grilla Buckler", EARTH, SWAP_FORCE, "Swap Force", 2013, "Earth buckler variant"},
    {0x0000004A, "Hoot Buckler", MAGIC, SWAP_FORCE, "Swap Force", 2013, "Magic buckler"},
    {0x0000004B, "Trap Buckler", UNDEAD, SWAP_FORCE, "Swap Force", 2013, "Undead buckler"},
    {0x0000004C, "Magna Buckler", TECH, SWAP_FORCE, "Swap Force", 2013, "Tech buckler"},
    {0x0000004D, "Spy Buckler", TECH, SWAP_FORCE, "Swap Force", 2013, "Tech buckler variant"},
    {0x0000004E, "Night Buckler", UNDEAD, SWAP_FORCE, "Swap Force", 2013, "Undead buckler variant"},
    {0x0000004F, "Rattle Buckler", UNDEAD, SWAP_FORCE, "Swap Force", 2013, "Undead buckler variant"},
    {0x00000050, "Freeze Buckler", WATER, SWAP_FORCE, "Swap Force", 2013, "Water buckler"},
    
    // Trap Team (2014)
    {0x00000051, "Snap Shot", WATER, TRAP_TEAM, "Trap Team", 2014, "Trap master with water powers"},
    {0x00000052, "Lob-Star", WATER, TRAP_TEAM, "Trap Team", 2014, "Lobster pirate"},
    {0x00000053, "Flip Wreck", WATER, TRAP_TEAM, "Trap Team", 2014, "Flip master"},
    {0x00000054, "Echo", WATER, TRAP_TEAM, "Trap Team", 2014, "Dolphin with sound powers"},
    {0x00000055, "Blastermind", MAGIC, TRAP_TEAM, "Trap Team", 2014, "Mind-controlling wizard"},
    {0x00000056, "Enigma", MAGIC, TRAP_TEAM, "Trap Team", 2014, "Mysterious sorcerer"},
    {0x00000057, "Deja Vu", MAGIC, TRAP_TEAM, "Trap Team", 2014, "Time-controlling mage"},
    {0x00000058, "Cobra Cadabra", MAGIC, TRAP_TEAM, "Trap Team", 2014, "Snake magician"},
    {0x00000059, "Jawbreaker", TECH, TRAP_TEAM, "Trap Team", 2014, "Robot with jaw attacks"},
    {0x0000005A, "Gearshift", TECH, TRAP_TEAM, "Trap Team", 2014, "Gear-shifting robot"},
    {0x0000005B, "Chopper", TECH, TRAP_TEAM, "Trap Team", 2014, "Helicopter robot"},
    {0x0000005C, "Tread Head", TECH, TRAP_TEAM, "Trap Team", 2014, "Tank robot"},
    {0x0000005D, "Bushwhack", LIFE, TRAP_TEAM, "Trap Team", 2014, "Plant warrior"},
    {0x0000005E, "Tuff Luck", LIFE, TRAP_TEAM, "Trap Team", 2014, "Lucky rabbit"},
    {0x0000005F, "Food Fight", LIFE, TRAP_TEAM, "Trap Team", 2014, "Food warrior"},
    {0x00000060, "Prism Break", EARTH, TRAP_TEAM, "Trap Team", 2014, "Crystal golem (re-release)"},
    
    // SuperChargers (2015)
    {0x00000061, "Spitfire", FIRE, SUPERCHARGERS, "SuperChargers", 2015, "Fire dragon with vehicle"},
    {0x00000062, "Hammer Slam Bowser", EARTH, SUPERCHARGERS, "SuperChargers", 2015, "Bowser crossover character"},
    {0x00000063, "Turbo Charge Donkey Kong", LIFE, SUPERCHARGERS, "SuperChargers", 2015, "Donkey Kong crossover character"},
    {0x00000064, "Fiesta", FIRE, SUPERCHARGERS, "SuperChargers", 2015, "Fire party character"},
    {0x00000065, "High Volt", TECH, SUPERCHARGERS, "SuperChargers", 2015, "Electric character"},
    {0x00000066, "Splat", WATER, SUPERCHARGERS, "SuperChargers", 2015, "Paint character"},
    {0x00000067, "Smash Hit", EARTH, SUPERCHARGERS, "SuperChargers", 2015, "Earth character"},
    {0x00000068, "Stormblade", AIR, SUPERCHARGERS, "SuperChargers", 2015, "Air character"},
    {0x00000069, "Thrillipede", UNDEAD, SUPERCHARGERS, "SuperChargers", 2015, "Undead character"},
    {0x0000006A, "Bone Bash Roller Brawl", UNDEAD, SUPERCHARGERS, "SuperChargers", 2015, "Undead roller character"},
    
    // Imaginators (2016)
    {0x0000006B, "King Pen", WATER, IMAGINATORS, "Imaginators", 2016, "King of Skylands"},
    {0x0000006C, "Golden Queen", EARTH, IMAGINATORS, "Imaginators", 2016, "Queen of Skylands"},
    {0x0000006D, "Grave Clobber", UNDEAD, IMAGINATORS, "Imaginators", 2016, "Undead warrior"},
    {0x0000006E, "Tidepool", WATER, IMAGINATORS, "Imaginators", 2016, "Water character"},
    {0x0000006F, "Ember", FIRE, IMAGINATORS, "Imaginators", 2016, "Fire character"},
    {0x00000070, "Starcast", MAGIC, IMAGINATORS, "Imaginators", 2016, "Magic character"},
    {0x00000071, "Mysticat", MAGIC, IMAGINATORS, "Imaginators", 2016, "Magic cat"},
    {0x00000072, "Blaster-Tron", TECH, IMAGINATORS, "Imaginators", 2016, "Tech character"},
    {0x00000073, "Chopscotch", UNDEAD, IMAGINATORS, "Imaginators", 2016, "Undead character"},
    {0x00000074, "Ro-Bow", TECH, IMAGINATORS, "Imaginators", 2016, "Tech bow character"},
    {0x00000075, "Wild Storm", AIR, IMAGINATORS, "Imaginators", 2016, "Air character"},
    {0x00000076, "Kaos", MAGIC, IMAGINATORS, "Imaginators", 2016, "Main villain"},
    {0x00000077, "Crash Bandicoot", LIFE, IMAGINATORS, "Imaginators", 2016, "Crash Bandicoot crossover"},
    {0x00000078, "Dr. Neo Cortex", TECH, IMAGINATORS, "Imaginators", 2016, "Dr. Cortex crossover"},
    
    // End marker
    {0xFFFFFFFF, "Unknown", UNKNOWN_ELEMENT, UNKNOWN_TYPE, "Unknown", 0, "Unknown Skylander"}
};

// Database size
#define DATABASE_SIZE (sizeof(skylander_database) / sizeof(skylander_database[0]))

const SkylanderDatabaseEntry* get_skylander_by_id(unsigned int skylander_id) {
    for (int i = 0; i < DATABASE_SIZE - 1; i++) { // -1 to exclude end marker
        if (skylander_database[i].id == skylander_id) {
            return &skylander_database[i];
        }
    }
    return &skylander_database[DATABASE_SIZE - 1]; // Return unknown entry
}

const char* get_skylander_name_by_id(unsigned int skylander_id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(skylander_id);
    return entry->name;
}

Element get_skylander_element_by_id(unsigned int skylander_id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(skylander_id);
    return entry->element;
}

SkylanderType get_skylander_type_by_id(unsigned int skylander_id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(skylander_id);
    return entry->type;
}

const char* get_skylander_game_by_id(unsigned int skylander_id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(skylander_id);
    return entry->game;
}

int get_skylander_year_by_id(unsigned int skylander_id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(skylander_id);
    return entry->year;
}

const char* get_skylander_description_by_id(unsigned int skylander_id) {
    const SkylanderDatabaseEntry* entry = get_skylander_by_id(skylander_id);
    return entry->description;
}

int get_total_skylanders_count(void) {
    return DATABASE_SIZE - 1; // Exclude end marker
}

int get_skylanders_by_element_count(Element element) {
    int count = 0;
    for (int i = 0; i < DATABASE_SIZE - 1; i++) {
        if (skylander_database[i].element == element) {
            count++;
        }
    }
    return count;
}

int get_skylanders_by_type_count(SkylanderType type) {
    int count = 0;
    for (int i = 0; i < DATABASE_SIZE - 1; i++) {
        if (skylander_database[i].type == type) {
            count++;
        }
    }
    return count;
}

void search_skylanders_by_name(const char* search_term) {
    printf("\n=== Skylanders matching '%s' ===\n", search_term);
    int found = 0;
    
    for (int i = 0; i < DATABASE_SIZE - 1; i++) {
        if (strstr(skylander_database[i].name, search_term) != NULL) {
            printf("%s (%s) - %s\n", 
                   skylander_database[i].name,
                   element_to_string(skylander_database[i].element),
                   skylander_database[i].game);
            found++;
        }
    }
    
    if (found == 0) {
        printf("No Skylanders found matching '%s'\n", search_term);
    } else {
        printf("\nFound %d Skylander(s)\n", found);
    }
}

void search_skylanders_by_element(Element element) {
    printf("\n=== %s Element Skylanders ===\n", element_to_string(element));
    int found = 0;
    
    for (int i = 0; i < DATABASE_SIZE - 1; i++) {
        if (skylander_database[i].element == element) {
            printf("%s (%s) - %s\n", 
                   skylander_database[i].name,
                   type_to_string(skylander_database[i].type),
                   skylander_database[i].game);
            found++;
        }
    }
    
    printf("\nFound %d %s Skylander(s)\n", found, element_to_string(element));
}

void search_skylanders_by_type(SkylanderType type) {
    printf("\n=== %s Skylanders ===\n", type_to_string(type));
    int found = 0;
    
    for (int i = 0; i < DATABASE_SIZE - 1; i++) {
        if (skylander_database[i].type == type) {
            printf("%s (%s) - %s\n", 
                   skylander_database[i].name,
                   element_to_string(skylander_database[i].element),
                   skylander_database[i].game);
            found++;
        }
    }
    
    printf("\nFound %d %s Skylander(s)\n", found, type_to_string(type));
}

// Interactive database functions
void search_database_interactive(void) {
    printf("\n=== Search Skylander Database ===\n");
    printf("1. Search by name\n");
    printf("2. Search by element\n");
    printf("3. Search by type\n");
    printf("0. Back to main menu\n");
    printf("Enter your choice: ");
    
    int choice;
    if (scanf("%d", &choice) != 1) {
        printf("Invalid input. Please enter a number.\n");
        while (getchar() != '\n'); // Clear input buffer
        return;
    }
    
    switch (choice) {
        case 1: {
            char search_term[50];
            printf("Enter Skylander name to search: ");
            while (getchar() != '\n'); // Clear input buffer
            fgets(search_term, sizeof(search_term), stdin);
            search_term[strcspn(search_term, "\n")] = 0; // Remove newline
            search_skylanders_by_name(search_term);
            break;
        }
        case 2: {
            printf("Select element:\n");
            printf("1. Fire\n2. Water\n3. Air\n4. Earth\n5. Life\n");
            printf("6. Undead\n7. Tech\n8. Magic\n9. Light\n10. Dark\n");
            printf("Enter element number: ");
            int element_choice;
            if (scanf("%d", &element_choice) == 1 && element_choice >= 1 && element_choice <= 10) {
                Element elements[] = {FIRE, WATER, AIR, EARTH, LIFE, UNDEAD, TECH, MAGIC, LIGHT, DARK};
                search_skylanders_by_element(elements[element_choice - 1]);
            } else {
                printf("Invalid element choice.\n");
            }
            break;
        }
        case 3: {
            printf("Select type:\n");
            printf("1. Giants\n2. Swap Force\n3. Trap Team\n4. SuperChargers\n5. Imaginators\n6. Spyro\n");
            printf("Enter type number: ");
            int type_choice;
            if (scanf("%d", &type_choice) == 1 && type_choice >= 1 && type_choice <= 6) {
                SkylanderType types[] = {GIANTS, SWAP_FORCE, TRAP_TEAM, SUPERCHARGERS, IMAGINATORS, SPYRO};
                search_skylanders_by_type(types[type_choice - 1]);
            } else {
                printf("Invalid type choice.\n");
            }
            break;
        }
        case 0:
            return;
        default:
            printf("Invalid choice. Please try again.\n");
    }
}

void database_statistics_interactive(void) {
    printf("\n=== Skylander Database Statistics ===\n");
    
    int total = get_total_skylanders_count();
    printf("Total Skylanders in database: %d\n\n", total);
    
    printf("Skylanders by Element:\n");
    printf("Fire: %d\n", get_skylanders_by_element_count(FIRE));
    printf("Water: %d\n", get_skylanders_by_element_count(WATER));
    printf("Air: %d\n", get_skylanders_by_element_count(AIR));
    printf("Earth: %d\n", get_skylanders_by_element_count(EARTH));
    printf("Life: %d\n", get_skylanders_by_element_count(LIFE));
    printf("Undead: %d\n", get_skylanders_by_element_count(UNDEAD));
    printf("Tech: %d\n", get_skylanders_by_element_count(TECH));
    printf("Magic: %d\n", get_skylanders_by_element_count(MAGIC));
    printf("Light: %d\n", get_skylanders_by_element_count(LIGHT));
    printf("Dark: %d\n", get_skylanders_by_element_count(DARK));
    
    printf("\nSkylanders by Type:\n");
    printf("Spyro's Adventure: %d\n", get_skylanders_by_type_count(SPYRO));
    printf("Giants: %d\n", get_skylanders_by_type_count(GIANTS));
    printf("Swap Force: %d\n", get_skylanders_by_type_count(SWAP_FORCE));
    printf("Trap Team: %d\n", get_skylanders_by_type_count(TRAP_TEAM));
    printf("SuperChargers: %d\n", get_skylanders_by_type_count(SUPERCHARGERS));
    printf("Imaginators: %d\n", get_skylanders_by_type_count(IMAGINATORS));
}
