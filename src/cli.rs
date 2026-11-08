use std::io::{self, Write};
use std::path::Path;
use std::time::Duration;

use crate::collection::Collection;
use crate::database::Database;
use crate::error::Error;
use crate::portal::Portal;
use crate::skylander::{Element, Skylander, SkylanderType};

const COLLECTION_FILE: &str = "collection.txt";
const PORTAL_WAIT: Duration = Duration::from_secs(30);

const MENU: &str = "\
=== Main Menu ===
1. Add Skylander
2. View Collection
3. Search Skylander
4. Remove Skylander
5. Save Collection
6. Load Collection
7. Connect Portal of Power
8. Read Skylander from Portal
9. Portal Status
10. Search Skylander Database
11. Database Statistics
0. Exit";

pub fn run() -> Result<(), Error> {
    println!("=== SkyArchive - Skylander Collection Manager ===");

    let mut collection = Collection::new();
    let mut portal = Portal::open();
    let database = match Database::load_default() {
        Ok(database) => {
            println!("Loaded {} Skylanders from database file", database.len());
            database
        }
        Err(err) => {
            println!(
                "Warning: Could not load Skylander database ({err}). Portal detection may not work properly."
            );
            Database::empty()
        }
    };

    loop {
        println!("\n{MENU}");
        let Some(choice) = prompt("Enter your choice: ")? else {
            println!("Thank you for using SkyArchive!");
            break;
        };
        match choice.parse::<u32>() {
            Ok(1) => add_skylander(&mut collection)?,
            Ok(2) => collection.print(),
            Ok(3) => search_collection(&collection)?,
            Ok(4) => remove_skylander(&mut collection)?,
            Ok(5) => save_collection(&collection),
            Ok(6) => load_collection(&mut collection),
            Ok(7) => connect_portal(&mut portal),
            Ok(8) => read_from_portal(&portal, &mut collection, &database)?,
            Ok(9) => portal_status(&portal),
            Ok(10) => search_database(&database)?,
            Ok(11) => print_database_stats(&database),
            Ok(0) => {
                println!("Thank you for using SkyArchive!");
                break;
            }
            Ok(_) => println!("Invalid choice. Please try again."),
            Err(_) => println!("Invalid input. Please enter a number."),
        }
    }
    Ok(())
}

fn prompt(message: &str) -> Result<Option<String>, Error> {
    print!("{message}");
    io::stdout().flush()?;
    let mut line = String::new();
    if io::stdin().read_line(&mut line)? == 0 {
        return Ok(None);
    }
    Ok(Some(line.trim().to_owned()))
}

fn add_skylander(collection: &mut Collection) -> Result<(), Error> {
    println!("\n=== Add New Skylander ===");
    let Some(name) = prompt("Enter Skylander name: ")? else {
        return Ok(());
    };
    let Some(element_text) = prompt(
        "Enter element (Fire, Water, Air, Earth, Life, Undead, Tech, Magic, Light, Dark, Kaos): ",
    )?
    else {
        return Ok(());
    };
    let Some(kind_text) = prompt(
        "Enter type (Core, Giants, Swap Force, Trap Team, SuperChargers, Imaginators, Mini, Villain, Sensei Villain): ",
    )?
    else {
        return Ok(());
    };
    let Some(game) = prompt("Enter game: ")? else {
        return Ok(());
    };
    let Some(year) = prompt_year()? else {
        return Ok(());
    };
    let Some(description) = prompt("Enter description: ")? else {
        return Ok(());
    };
    let Some(owned_text) = prompt("Do you own this Skylander? (yes/no): ")? else {
        return Ok(());
    };

    let element = match element_text.parse() {
        Ok(element) => element,
        Err(_) => {
            println!("Unknown element '{element_text}'. Saving as Unknown.");
            Element::Unknown
        }
    };
    let kind = match kind_text.parse() {
        Ok(kind) => kind,
        Err(_) => {
            println!("Unknown type '{kind_text}'. Saving as Unknown.");
            SkylanderType::Unknown
        }
    };
    let mut figure = Skylander::new(name, element, kind, game, year, description);
    figure.owned = matches!(owned_text.to_ascii_lowercase().as_str(), "y" | "yes");
    acknowledge_add(collection, figure);
    Ok(())
}

fn prompt_year() -> Result<Option<i32>, Error> {
    loop {
        let Some(text) = prompt("Enter year: ")? else {
            return Ok(None);
        };
        if let Ok(year) = text.parse() {
            return Ok(Some(year));
        }
        println!("Enter a whole number for the year.");
    }
}

fn acknowledge_add(collection: &mut Collection, figure: Skylander) {
    let name = figure.name.clone();
    if collection.add(figure).is_ok() {
        println!("Successfully added '{name}' to collection!");
    } else {
        println!("Skylander '{name}' already exists in collection!");
    }
}

fn search_collection(collection: &Collection) -> Result<(), Error> {
    println!("\n=== Search Skylander ===");
    let Some(name) = prompt("Enter Skylander name to search: ")? else {
        return Ok(());
    };
    if let Some(figure) = collection.find(&name) {
        figure.print_detailed();
    } else {
        println!("Skylander '{name}' not found in collection!");
    }
    Ok(())
}

fn remove_skylander(collection: &mut Collection) -> Result<(), Error> {
    println!("\n=== Remove Skylander ===");
    let Some(name) = prompt("Enter Skylander name to remove: ")? else {
        return Ok(());
    };
    if collection.remove(&name).is_some() {
        println!("Successfully removed '{name}' from collection!");
    } else {
        println!("Skylander '{name}' not found in collection!");
    }
    Ok(())
}

fn save_collection(collection: &Collection) {
    let path = Path::new(COLLECTION_FILE);
    match collection.save(path) {
        Ok(()) => println!("Collection saved to '{COLLECTION_FILE}' successfully!"),
        Err(err) => println!("{err}"),
    }
}

fn load_collection(collection: &mut Collection) {
    let path = Path::new(COLLECTION_FILE);
    match Collection::load(path) {
        Ok(loaded) => {
            let count = loaded.len();
            *collection = loaded;
            println!(
                "Collection loaded from '{COLLECTION_FILE}' successfully! ({count} Skylanders)"
            );
        }
        Err(err) => println!("{err}"),
    }
}

fn connect_portal(portal: &mut Portal) {
    println!("\n=== Connect Portal of Power ===");
    if portal.is_connected() {
        println!("Portal is already connected!");
        return;
    }
    println!("Searching for Portal of Power...");
    portal.print_devices();
    if portal.connect().is_ok() {
        println!("Portal connected successfully!");
    } else {
        println!("Failed to connect to Portal of Power.");
        println!("Make sure:");
        println!("- Portal is connected via USB");
        println!("- You have the necessary permissions (try running as administrator/sudo)");
        println!("- Portal is compatible (PS3/PS4/Wii/WiiU/PC portals work best)");
    }
}

fn read_from_portal(
    portal: &Portal,
    collection: &mut Collection,
    database: &Database,
) -> Result<(), Error> {
    println!("\n=== Read Skylander from Portal ===");
    if !portal.is_connected() {
        println!("Error: Portal not connected! Please connect first (option 7).");
        return Ok(());
    }
    println!("Place a Skylander on the Portal of Power...");
    match portal.wait_for_skylander(database, PORTAL_WAIT) {
        Ok(figure) => {
            println!("\nSkylander detected and read successfully!");
            figure.print_detailed();
            let Some(answer) =
                prompt("\nWould you like to add this Skylander to your collection? (y/n): ")?
            else {
                return Ok(());
            };
            if matches!(answer.to_ascii_lowercase().as_str(), "y" | "yes") {
                acknowledge_add(collection, figure);
            }
        }
        Err(_) => println!("No Skylander detected or error occurred."),
    }
    Ok(())
}

fn portal_status(portal: &Portal) {
    println!("\n=== Portal Status ===");
    portal.print_info();
    if !portal.is_connected() {
        println!("\nPortal is not connected.");
        return;
    }
    println!("\nTesting portal communication...");
    match portal.detect() {
        Ok(true) => println!("Status: Portal is working - Skylander detected!"),
        Ok(false) => println!("Status: Portal is working - No Skylander detected."),
        Err(_) => println!("Status: Portal communication error."),
    }
}

fn search_database(database: &Database) -> Result<(), Error> {
    println!("\n=== Search Skylander Database ===");
    println!("1. Search by name");
    println!("2. Search by element");
    println!("3. Search by type");
    let Some(choice) = prompt("Enter your choice: ")? else {
        return Ok(());
    };
    let results: Vec<_> = match choice.parse::<u32>() {
        Ok(1) => {
            let Some(name) = prompt("Enter name to search: ")? else {
                return Ok(());
            };
            if name.is_empty() {
                println!("Enter a name to search.");
                return Ok(());
            }
            database.search_name(&name)
        }
        Ok(2) => {
            let Some(text) = prompt(
                "Enter element (Fire, Water, Air, Earth, Life, Undead, Tech, Magic, Light, Dark, Kaos): ",
            )?
            else {
                return Ok(());
            };
            match text.parse() {
                Ok(element) => database.search_element(element),
                Err(_) => {
                    println!("Unknown element '{text}'.");
                    return Ok(());
                }
            }
        }
        Ok(3) => {
            let Some(text) = prompt(
                "Enter type (Core, Giant, Swap Force, Trap Master, SuperCharger, Sensei, Mini, Villain, Sensei Villain): ",
            )?
            else {
                return Ok(());
            };
            match text.parse() {
                Ok(kind) => database.search_kind(kind),
                Err(_) => {
                    println!("Unknown type '{text}'.");
                    return Ok(());
                }
            }
        }
        _ => {
            println!("Invalid choice");
            return Ok(());
        }
    };

    if results.is_empty() {
        println!("No Skylanders found matching your criteria.");
    } else {
        println!("\nFound {} Skylanders:", results.len());
        for (index, entry) in results.iter().enumerate() {
            println!(
                "{}. {} ({}, {}, {})",
                index + 1,
                entry.name,
                entry.element,
                entry.game,
                entry.year
            );
        }
    }
    Ok(())
}

fn print_database_stats(database: &Database) {
    println!("\n=== Skylander Database Statistics ===");
    println!("Total Skylanders: {}", database.len());
    println!("\nBy Element:");
    for element in Element::STATS {
        println!("{element}: {}", database.count_element(element));
    }
    println!("\nBy Type:");
    for kind in SkylanderType::STATS {
        println!("{}: {}", kind.stats_label(), database.count_kind(kind));
    }
}
