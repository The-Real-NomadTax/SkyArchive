use std::fmt;
use std::str::FromStr;

/// Element printed in collection files and on screen.
///
/// Declaration order is the Skylanders element order. Sorting by element uses it.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub enum Element {
    Fire,
    Water,
    Air,
    Earth,
    Life,
    Undead,
    Tech,
    Magic,
    Light,
    Dark,
    Kaos,
    Unknown,
}

impl Element {
    pub const STATS: [Self; 11] = [
        Self::Fire,
        Self::Water,
        Self::Earth,
        Self::Air,
        Self::Life,
        Self::Magic,
        Self::Tech,
        Self::Undead,
        Self::Light,
        Self::Dark,
        Self::Kaos,
    ];

    pub const fn as_str(self) -> &'static str {
        match self {
            Self::Fire => "Fire",
            Self::Water => "Water",
            Self::Air => "Air",
            Self::Earth => "Earth",
            Self::Life => "Life",
            Self::Undead => "Undead",
            Self::Tech => "Tech",
            Self::Magic => "Magic",
            Self::Light => "Light",
            Self::Dark => "Dark",
            Self::Kaos => "Kaos",
            Self::Unknown => "Unknown",
        }
    }
}

impl fmt::Display for Element {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        formatter.write_str(self.as_str())
    }
}

impl FromStr for Element {
    type Err = ParseLabelError;

    fn from_str(text: &str) -> Result<Self, Self::Err> {
        const LABELS: &[(&str, Element)] = &[
            ("fire", Element::Fire),
            ("water", Element::Water),
            ("air", Element::Air),
            ("earth", Element::Earth),
            ("life", Element::Life),
            ("undead", Element::Undead),
            ("tech", Element::Tech),
            ("magic", Element::Magic),
            ("light", Element::Light),
            ("dark", Element::Dark),
            ("kaos", Element::Kaos),
            ("unknown", Element::Unknown),
        ];
        LABELS
            .iter()
            .find(|(label, _)| words_eq(text, label))
            .map(|(_, element)| *element)
            .ok_or(ParseLabelError)
    }
}

/// Figure variant. `Spyro` is a Core Skylander from the first game.
#[derive(Debug, Clone, Copy, PartialEq, Eq, PartialOrd, Ord, Hash)]
pub enum SkylanderType {
    Giants,
    SwapForce,
    TrapTeam,
    SuperChargers,
    Imaginators,
    Spyro,
    Mini,
    Villain,
    SenseiVillain,
    Unknown,
}

impl SkylanderType {
    /// Statistics screen order and the names that screen prints.
    pub const STATS: [Self; 9] = [
        Self::Spyro,
        Self::Giants,
        Self::SwapForce,
        Self::TrapTeam,
        Self::SuperChargers,
        Self::Imaginators,
        Self::Mini,
        Self::Villain,
        Self::SenseiVillain,
    ];

    /// Token written to collection files. Kept stable for files saved by the C build.
    pub const fn as_str(self) -> &'static str {
        match self {
            Self::Giants => "Giants",
            Self::SwapForce => "Swap Force",
            Self::TrapTeam => "Trap Team",
            Self::SuperChargers => "SuperChargers",
            Self::Imaginators => "Imaginators",
            Self::Spyro => "Spyro",
            Self::Mini => "Mini",
            Self::Villain => "Villain",
            Self::SenseiVillain => "Sensei Villain",
            Self::Unknown => "Unknown",
        }
    }

    pub const fn stats_label(self) -> &'static str {
        match self {
            Self::Spyro => "Core",
            Self::Giants => "Giant",
            Self::SwapForce => "Swap Force",
            Self::TrapTeam => "Trap Master",
            Self::SuperChargers => "SuperCharger",
            Self::Imaginators => "Sensei",
            Self::Mini => "Mini",
            Self::Villain => "Villain",
            Self::SenseiVillain => "Sensei Villain",
            Self::Unknown => "Unknown",
        }
    }
}

impl fmt::Display for SkylanderType {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        formatter.write_str(self.as_str())
    }
}

impl FromStr for SkylanderType {
    type Err = ParseLabelError;

    fn from_str(text: &str) -> Result<Self, Self::Err> {
        // Database dumps and the statistics screen use a second set of names.
        const LABELS: &[(&str, SkylanderType)] = &[
            ("giants", SkylanderType::Giants),
            ("giant", SkylanderType::Giants),
            ("swap force", SkylanderType::SwapForce),
            ("swapforce", SkylanderType::SwapForce),
            ("trap team", SkylanderType::TrapTeam),
            ("trap master", SkylanderType::TrapTeam),
            ("trapmaster", SkylanderType::TrapTeam),
            ("superchargers", SkylanderType::SuperChargers),
            ("supercharger", SkylanderType::SuperChargers),
            ("imaginators", SkylanderType::Imaginators),
            ("sensei", SkylanderType::Imaginators),
            ("spyro", SkylanderType::Spyro),
            ("core", SkylanderType::Spyro),
            ("mini", SkylanderType::Mini),
            ("villain", SkylanderType::Villain),
            ("sensei villain", SkylanderType::SenseiVillain),
            ("unknown", SkylanderType::Unknown),
        ];
        LABELS
            .iter()
            .find(|(label, _)| words_eq(text, label))
            .map(|(_, kind)| *kind)
            .ok_or(ParseLabelError)
    }
}

#[derive(Debug, Clone, Copy, PartialEq, Eq)]
pub struct ParseLabelError;

impl fmt::Display for ParseLabelError {
    fn fmt(&self, formatter: &mut fmt::Formatter<'_>) -> fmt::Result {
        formatter.write_str("unknown label")
    }
}

impl std::error::Error for ParseLabelError {}

fn words_eq(input: &str, canonical: &str) -> bool {
    let mut input = input.split_whitespace();
    let mut canonical = canonical.split_whitespace();
    loop {
        match (input.next(), canonical.next()) {
            (Some(left), Some(right)) if left.eq_ignore_ascii_case(right) => {}
            (None, None) => return true,
            _ => return false,
        }
    }
}

/// One figure in a person's collection.
#[derive(Debug, Clone, PartialEq, Eq)]
pub struct Skylander {
    pub name: String,
    pub element: Element,
    pub kind: SkylanderType,
    pub game: String,
    pub year: i32,
    pub description: String,
    pub owned: bool,
}

impl Skylander {
    pub fn new(
        name: impl Into<String>,
        element: Element,
        kind: SkylanderType,
        game: impl Into<String>,
        year: i32,
        description: impl Into<String>,
    ) -> Self {
        Self {
            name: name.into(),
            element,
            kind,
            game: game.into(),
            year,
            description: description.into(),
            owned: false,
        }
    }

    pub fn print_detailed(&self) {
        println!();
        println!("=== {} ===", self.name);
        println!("Element: {}", self.element);
        println!("Type: {}", self.kind);
        println!("Game: {}", self.game);
        println!("Year: {}", self.year);
        println!("Status: {}", if self.owned { "Owned" } else { "Not Owned" });
        println!("Description: {}", self.description);
        println!("========================");
    }

    pub fn print_brief(&self) {
        println!(
            "{:<20} | {:<12} | {:<15} | {:<20} | {}",
            self.name,
            self.element,
            self.kind,
            self.game,
            if self.owned { "Owned" } else { "Not Owned" }
        );
    }
}

#[cfg(test)]
mod tests {
    use super::*;

    #[test]
    fn parses_labels_and_aliases() {
        assert_eq!("Fire".parse(), Ok(Element::Fire));
        assert_eq!("kaos".parse(), Ok(Element::Kaos));
        assert_eq!("light".parse::<Element>(), Ok(Element::Light));
        assert!("plasma".parse::<Element>().is_err());

        assert_eq!("Core".parse(), Ok(SkylanderType::Spyro));
        assert_eq!("Giant".parse(), Ok(SkylanderType::Giants));
        assert_eq!("Trap Master".parse(), Ok(SkylanderType::TrapTeam));
        assert_eq!("SuperCharger".parse(), Ok(SkylanderType::SuperChargers));
        assert_eq!("Sensei".parse(), Ok(SkylanderType::Imaginators));
        assert_eq!("Sensei Villain".parse(), Ok(SkylanderType::SenseiVillain));
        assert_eq!(SkylanderType::Spyro.as_str(), "Spyro");
        assert_eq!(SkylanderType::Spyro.stats_label(), "Core");
    }
}
