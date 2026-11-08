use std::collections::HashMap;
use std::fs;
use std::path::PathBuf;

use crate::error::Error;
use crate::skylander::{Element, Skylander, SkylanderType};

/// First synthetic id assigned while reading a SQL dump.
///
/// The dump has no real NFC ids. Rows are numbered from Eruptor's id (`0xB7`)
/// so a portal report of that form can still resolve the first row.
const FIRST_FIGURE_ID: u32 = 0x0000_00B7;

const SQL_NAME: &str = "skylanders.db.sql";

/// One row from the Skylander SQL dump.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct DatabaseEntry {
    pub id: u32,
    pub name: String,
    pub element: Element,
    pub kind: SkylanderType,
    pub game: String,
    pub year: i32,
    pub description: String,
}

/// Figures loaded from `skylanders.db.sql`.
#[derive(Debug, Clone, Default)]
pub struct Database {
    entries: Vec<DatabaseEntry>,
    by_id: HashMap<u32, usize>,
}

impl Database {
    pub fn empty() -> Self {
        Self::default()
    }

    pub fn len(&self) -> usize {
        self.entries.len()
    }

    pub fn is_empty(&self) -> bool {
        self.entries.is_empty()
    }

    pub fn get(&self, id: u32) -> Option<&DatabaseEntry> {
        self.by_id.get(&id).map(|index| &self.entries[*index])
    }

    pub fn skylander_for_id(&self, id: u32) -> (Skylander, bool) {
        if let Some(entry) = self.get(id) {
            (
                Skylander {
                    name: entry.name.clone(),
                    element: entry.element,
                    kind: entry.kind,
                    game: entry.game.clone(),
                    year: entry.year,
                    description: entry.description.clone(),
                    owned: true,
                },
                true,
            )
        } else {
            (
                Skylander {
                    name: format!("Unknown_{id:08X}"),
                    element: Element::Unknown,
                    kind: SkylanderType::Unknown,
                    game: "Unknown".to_owned(),
                    year: 0,
                    description: "Unknown Skylander".to_owned(),
                    owned: true,
                },
                false,
            )
        }
    }

    pub fn count_element(&self, element: Element) -> usize {
        self.entries
            .iter()
            .filter(|entry| entry.element == element)
            .count()
    }

    pub fn count_kind(&self, kind: SkylanderType) -> usize {
        self.entries
            .iter()
            .filter(|entry| entry.kind == kind)
            .count()
    }

    pub fn search_name(&self, query: &str) -> Vec<&DatabaseEntry> {
        let query = query.to_lowercase();
        self.entries
            .iter()
            .filter(|entry| entry.name.to_lowercase().contains(&query))
            .collect()
    }

    pub fn search_element(&self, element: Element) -> Vec<&DatabaseEntry> {
        self.entries
            .iter()
            .filter(|entry| entry.element == element)
            .collect()
    }

    pub fn search_kind(&self, kind: SkylanderType) -> Vec<&DatabaseEntry> {
        self.entries
            .iter()
            .filter(|entry| entry.kind == kind)
            .collect()
    }

    /// Loads the first `skylanders.db.sql` found in the working directory,
    /// its parent, or beside the executable.
    pub fn load_default() -> Result<Self, Error> {
        let path = locate_sql_file()?;
        let text = fs::read_to_string(&path)
            .map_err(|_| Error::message(format!("could not open '{}'", path.display())))?;
        Self::from_sql(&text)
    }

    pub fn from_sql(sql: &str) -> Result<Self, Error> {
        let sql = sql.strip_prefix('\u{feff}').unwrap_or(sql);
        let mut database = Self::empty();
        for line in sql.lines() {
            if !line.contains("INSERT INTO skylanders") {
                continue;
            }
            let Some((name, element, game, year, kind)) = parse_values(line) else {
                continue;
            };
            let id = FIRST_FIGURE_ID.saturating_add(database.entries.len() as u32);
            database.push(DatabaseEntry {
                id,
                element: element.parse().unwrap_or(Element::Unknown),
                kind: kind.parse().unwrap_or(SkylanderType::Unknown),
                description: name.clone(),
                name,
                game,
                year,
            });
        }
        if database.is_empty() {
            return Err(Error::message("no Skylander data found in file"));
        }
        Ok(database)
    }

    fn push(&mut self, entry: DatabaseEntry) {
        let id = entry.id;
        let index = self.entries.len();
        self.entries.push(entry);
        self.by_id.insert(id, index);
    }
}

fn locate_sql_file() -> Result<PathBuf, Error> {
    let mut candidates = vec![PathBuf::from(SQL_NAME), PathBuf::from("..").join(SQL_NAME)];
    if let Ok(exe) = std::env::current_exe() {
        if let Some(dir) = exe.parent() {
            candidates.push(dir.join(SQL_NAME));
            if let Some(parent) = dir.parent() {
                candidates.push(parent.join(SQL_NAME));
            }
        }
    }
    candidates
        .into_iter()
        .find(|path| path.is_file())
        .ok_or_else(|| Error::message(format!("could not find {SQL_NAME}")))
}

fn parse_values(line: &str) -> Option<(String, String, String, i32, String)> {
    let rest = find_values(line)?.trim_start().strip_prefix('(')?;
    let (name, rest) = parse_sql_string(rest)?;
    let rest = skip_comma(rest)?;
    let (element, rest) = parse_sql_string(rest)?;
    let rest = skip_comma(rest)?;
    let (game, rest) = parse_sql_string(rest)?;
    let rest = skip_comma(rest)?;
    let (year, rest) = parse_sql_int(rest)?;
    let rest = skip_comma(rest)?;
    let (kind, _) = parse_sql_string(rest)?;
    Some((name, element, game, year, kind))
}

fn find_values(line: &str) -> Option<&str> {
    let lower = line.to_ascii_lowercase();
    let index = lower.find("values")?;
    Some(&line[index + "values".len()..])
}

fn skip_comma(input: &str) -> Option<&str> {
    input.trim_start().strip_prefix(',')
}

fn parse_sql_string(input: &str) -> Option<(String, &str)> {
    let input = input.trim_start();
    let bytes = input.as_bytes();
    if bytes.first().copied() != Some(b'\'') {
        return None;
    }
    let mut out = String::new();
    let mut index = 1;
    while index < bytes.len() {
        if bytes[index] == b'\'' {
            if bytes.get(index + 1) == Some(&b'\'') {
                out.push('\'');
                index += 2;
                continue;
            }
            return Some((out, &input[index + 1..]));
        }
        let ch = input[index..].chars().next()?;
        out.push(ch);
        index += ch.len_utf8();
    }
    None
}

fn parse_sql_int(input: &str) -> Option<(i32, &str)> {
    let input = input.trim_start();
    let bytes = input.as_bytes();
    let mut index = 0;
    if matches!(bytes.first(), Some(b'+' | b'-')) {
        index += 1;
    }
    let digits_at = index;
    while bytes.get(index).is_some_and(u8::is_ascii_digit) {
        index += 1;
    }
    if index == digits_at {
        return None;
    }
    let year = input[..index].parse().ok()?;
    Some((year, &input[index..]))
}

#[cfg(test)]
mod tests {
    use super::*;

    const DUMP: &str = "\
CREATE TABLE skylanders (name TEXT);
INSERT INTO skylanders (name, element, debut_game, year, type) VALUES ('Eruptor', 'Fire', 'Spyro''s Adventure', 2011, 'Core');
INSERT INTO skylanders (name, element, debut_game, year, type) VALUES ('Wash Buckler', 'Water', 'Swap Force', 2013, 'Swap Force');
-- not a row
";

    #[test]
    fn parses_inserts_with_escaped_quotes_and_synthetic_ids() {
        let database = Database::from_sql(DUMP).unwrap();
        assert_eq!(database.len(), 2);
        let eruptor = database.get(0xB7).unwrap();
        assert_eq!(eruptor.name, "Eruptor");
        assert_eq!(eruptor.element, Element::Fire);
        assert_eq!(eruptor.kind, SkylanderType::Spyro);
        assert_eq!(eruptor.game, "Spyro's Adventure");
        assert_eq!(eruptor.year, 2011);
        assert_eq!(eruptor.description, "Eruptor");
        assert_eq!(database.get(0xB8).unwrap().name, "Wash Buckler");
        assert_eq!(database.count_element(Element::Fire), 1);
        assert_eq!(database.count_kind(SkylanderType::SwapForce), 1);
        assert_eq!(database.search_name("erupt").len(), 1);
        assert!(database.search_name("missing").is_empty());

        let (known, is_known) = database.skylander_for_id(0xB7);
        assert!(is_known);
        assert!(known.owned);
        assert_eq!(known.name, "Eruptor");

        let (unknown, is_known) = database.skylander_for_id(0x00FF);
        assert!(!is_known);
        assert_eq!(unknown.name, "Unknown_000000FF");
        assert_eq!(unknown.game, "Unknown");
    }

    #[test]
    fn rejects_a_dump_with_no_rows() {
        let err = Database::from_sql("SELECT 1;").unwrap_err();
        assert!(err.to_string().contains("no Skylander data"));
    }
}
