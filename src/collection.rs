use std::fs;
use std::path::Path;

use crate::error::Error;
use crate::skylander::{Element, Skylander, SkylanderType};

/// How many figures are owned versus listed.
#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct Stats {
    pub total: usize,
    pub owned: usize,
}

/// The figures someone is tracking.
#[derive(Debug, Clone, Default, PartialEq, Eq)]
pub struct Collection {
    skylanders: Vec<Skylander>,
}

impl Collection {
    pub fn new() -> Self {
        Self::default()
    }

    pub fn len(&self) -> usize {
        self.skylanders.len()
    }

    pub fn is_empty(&self) -> bool {
        self.skylanders.is_empty()
    }

    pub fn iter(&self) -> impl Iterator<Item = &Skylander> {
        self.skylanders.iter()
    }

    pub fn find(&self, name: &str) -> Option<&Skylander> {
        self.skylanders.iter().find(|figure| figure.name == name)
    }

    /// Adds `skylander` when no figure with that name is already stored.
    pub fn add(&mut self, skylander: Skylander) -> Result<(), Skylander> {
        if self.find(&skylander.name).is_some() {
            return Err(skylander);
        }
        self.skylanders.push(skylander);
        Ok(())
    }

    pub fn remove(&mut self, name: &str) -> Option<Skylander> {
        let index = self
            .skylanders
            .iter()
            .position(|figure| figure.name == name)?;
        Some(self.skylanders.remove(index))
    }

    pub fn stats(&self) -> Stats {
        let owned = self.skylanders.iter().filter(|figure| figure.owned).count();
        Stats {
            total: self.skylanders.len(),
            owned,
        }
    }

    pub fn sort_by_name(&mut self) {
        self.skylanders
            .sort_by(|left, right| left.name.cmp(&right.name));
    }

    /// Orders figures by Skylanders element order, then by name.
    pub fn sort_by_element(&mut self) {
        self.skylanders.sort_by(|left, right| {
            left.element
                .cmp(&right.element)
                .then_with(|| left.name.cmp(&right.name))
        });
    }

    pub fn print(&self) {
        if self.is_empty() {
            println!("Collection is empty!");
            return;
        }

        println!("\n=== Your Skylander Collection ===");
        println!("Total Skylanders: {}\n", self.len());
        println!(
            "Name                 | Element      | Type            | Game                 | Status"
        );
        println!("{}", "-".repeat(77));
        for figure in &self.skylanders {
            figure.print_brief();
        }

        let stats = self.stats();
        let percent = if stats.total > 0 {
            (stats.owned as f64) / (stats.total as f64) * 100.0
        } else {
            0.0
        };
        println!(
            "\nStatistics: {} owned out of {} total ({percent:.1}%)",
            stats.owned, stats.total
        );
    }

    /// Writes the collection in the original text format:
    /// a count, then `name|element|type|game|year|owned|description` lines.
    pub fn save(&self, path: &Path) -> Result<(), Error> {
        let mut contents = String::new();
        contents.push_str(&self.skylanders.len().to_string());
        contents.push('\n');
        for figure in &self.skylanders {
            contents.push_str(&format!(
                "{}|{}|{}|{}|{}|{}|{}\n",
                figure.name,
                figure.element,
                figure.kind,
                figure.game,
                figure.year,
                u8::from(figure.owned),
                figure.description
            ));
        }
        fs::write(path, contents).map_err(|_| {
            Error::message(format!(
                "Error: Could not open file '{}' for writing!",
                path.display()
            ))
        })
    }

    pub fn load(path: &Path) -> Result<Self, Error> {
        let text = fs::read_to_string(path).map_err(|_| {
            Error::message(format!(
                "Error: Could not open file '{}' for reading!",
                path.display()
            ))
        })?;
        Self::from_text(&text)
    }

    pub fn from_text(text: &str) -> Result<Self, Error> {
        let mut lines = text.lines();
        let header = lines
            .next()
            .ok_or_else(|| Error::message("Error: Invalid file format!"))?;
        let count: usize = header
            .split_whitespace()
            .next()
            .and_then(|token| token.parse().ok())
            .ok_or_else(|| Error::message("Error: Invalid file format!"))?;

        let mut skylanders = Vec::with_capacity(count);
        for line in lines.take(count) {
            if line.trim().is_empty() {
                continue;
            }
            if let Some(figure) = parse_record(line) {
                skylanders.push(figure);
            }
        }
        Ok(Self { skylanders })
    }
}

fn parse_record(line: &str) -> Option<Skylander> {
    let mut parts = line.splitn(7, '|');
    let name = parts.next()?;
    let element = parts.next()?;
    let kind = parts.next()?;
    let game = parts.next()?;
    let year = parts.next()?;
    let owned = parts.next()?;
    let description = parts.next()?;
    let year = year.trim().parse().ok()?;
    let owned = owned.trim().parse::<i32>().ok()? != 0;
    Some(Skylander {
        name: name.to_owned(),
        element: element.parse().unwrap_or(Element::Unknown),
        kind: kind.parse().unwrap_or(SkylanderType::Unknown),
        game: game.to_owned(),
        year,
        description: description.to_owned(),
        owned,
    })
}

#[cfg(test)]
mod tests {
    use super::*;

    fn sample() -> Collection {
        let mut collection = Collection::new();
        let mut eruptor = Skylander::new(
            "Eruptor",
            Element::Fire,
            SkylanderType::Spyro,
            "Spyro's Adventure",
            2011,
            "Lava monster",
        );
        eruptor.owned = true;
        let bash = Skylander::new(
            "Bash",
            Element::Earth,
            SkylanderType::Giants,
            "Giants",
            2012,
            "Rock|roll",
        );
        collection.add(eruptor).unwrap();
        collection.add(bash).unwrap();
        collection
    }

    #[test]
    fn rejects_a_duplicate_name_and_removes_by_name() {
        let mut collection = sample();
        let duplicate =
            Skylander::new("Eruptor", Element::Fire, SkylanderType::Spyro, "", 2011, "");
        assert!(collection.add(duplicate).is_err());
        assert!(collection.remove("Eruptor").is_some());
        assert!(collection.find("Eruptor").is_none());
        assert_eq!(collection.len(), 1);
    }

    #[test]
    fn round_trips_the_text_format() {
        let collection = sample();
        let text = {
            let dir = std::env::temp_dir().join(format!("skyarchive-test-{}", std::process::id()));
            fs::create_dir_all(&dir).unwrap();
            let path = dir.join("collection.txt");
            collection.save(&path).unwrap();
            let loaded = Collection::load(&path).unwrap();
            fs::remove_file(&path).ok();
            loaded
        };
        assert_eq!(text, collection);
        assert!(text.find("Bash").unwrap().description.contains('|'));
        assert!(text.find("Eruptor").unwrap().owned);
        assert!(!text.find("Bash").unwrap().owned);
    }

    #[test]
    fn sorts_by_name() {
        let mut collection = sample();
        collection.sort_by_name();
        let names: Vec<_> = collection
            .iter()
            .map(|figure| figure.name.as_str())
            .collect();
        assert_eq!(names, ["Bash", "Eruptor"]);
    }

    #[test]
    fn sorts_by_element_order_then_name() {
        let mut collection = sample();
        let mut spyro = Skylander::new(
            "Spyro",
            Element::Fire,
            SkylanderType::Spyro,
            "Spyro's Adventure",
            2011,
            "",
        );
        spyro.owned = true;
        collection.add(spyro).unwrap();
        collection.sort_by_element();
        let names: Vec<_> = collection
            .iter()
            .map(|figure| figure.name.as_str())
            .collect();
        assert_eq!(names, ["Eruptor", "Spyro", "Bash"]);
    }

    #[test]
    fn keeps_the_caller_collection_when_the_file_is_invalid() {
        let original = sample();
        let err = Collection::from_text("not a collection").unwrap_err();
        assert!(err.to_string().contains("Invalid file format"));
        assert_eq!(original.len(), 2);
    }
}
