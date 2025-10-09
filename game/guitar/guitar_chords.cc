#include "guitar_chords.hh"

#include <stdexcept>

auto GuitarChords::createChordMap() const -> std::map<std::string, Fingering> {
    auto map = std::map<std::string, Fingering>{
       {"E", {0, 2, 2, 1, 0, 0}},
        {"Em", {0, 2, 2, 0, 0, 0}},
        {"A", {-1, 0, 2, 2, 2, 0}},
        {"Am", {-1, 0, 2, 2, 1, 0}},
        {"C", {-1, 3, 2, 0, 1, 0}},
        {"Cm", {-1, 3, 5, 5, 4, 3}}, // C-Moll (Barré 3)
        {"G", {3, 2, 0, 0, 0, 3}},
        {"Gm", {3, 5, 5, 3, 3, 3}}, // G-Moll (Barré 3)
        {"D", {-1, -1, 0, 2, 3, 2}},
        {"Dm", {-1, -1, 0, 2, 3, 1}},
        {"F", {1, 3, 3, 2, 1, 1}},
        {"Fm", {1, 3, 3, 1, 1, 1}},
        {"B", {2, 2, 4, 4, 4, 2}},
        {"Bm", {2, 2, 4, 4, 3, 2}},
        {"Bb", {-1, 1, 3, 3, 3, 1}},
        {"Bbm", {-1, 1, 3, 3, 2, 1}},

        // --- Septakkorde (Dominant-7) ---
        {"E7", {0, 2, 0, 1, 0, 0}},
        {"A7", {-1, 0, 2, 0, 2, 0}},
        {"D7", {-1, -1, 0, 2, 1, 2}},

        // --- Moll-Septakkorde (m7) ---

        {"Am7", {-1, 0, 2, 0, 1, 0}},
        {"Em7", {0, 2, 0, 0, 0, 0}},
        {"Dm7", {-1, -1, 0, 2, 1, 1}},
        {"Cm7", {-1, 3, 5, 3, 4, 3}},
        {"Gm7", {3, 5, 3, 3, 3, 3}},
        {"Fm7", {1, 3, 1, 1, 1, 1}},
        {"Hm7", {-1, 2, 4, 2, 3, 2}}
    };

    return map;
}

auto GuitarChords::getFingering(const std::string& chord) const -> Fingering {
    if (const auto it = chordMap.find(chord); it != chordMap.end()) {
        return it->second;
    }

    throw std::out_of_range("Akkord '" + chord + "' nicht gefunden.");
}