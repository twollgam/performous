#include "guitar_chords.hh"

#include <stdexcept>

auto GuitarChords::createChordMap() const -> std::map<std::string, Fingering> {
    auto map = std::map<std::string, Fingering>{
        // E-Dur (E)
        {"E", { 0, 2, 2, 1, 0, 0 }},
        // E-Moll (Em)
        {"Em", { 0, 2, 2, 0, 0, 0 }},
        // A-Dur (A)
        {"A", { -1, 0, 2, 2, 2, 0 }},
        // A-Moll (Am)
        {"Am", { -1, 0, 2, 2, 1, 0 }},
        // C-Dur (C)
        {"C", { -1, 3, 2, 0, 1, 0 }},
        // G-Dur (G)
        {"G", { 3, 2, 0, 0, 0, 3 }},
        // D-Dur (D)
        {"D", { -1, -1, 0, 2, 3, 2 }},
        // D-Septime (D7)
        {"D7", { -1, -1, 0, 2, 1, 2 }},
        // D-Moll (Dm): E(x), A(x), D(0), G(2), B(3), E2(1)
        {"Dm", { -1, -1, 0, 2, 3, 1 }},
        // F-Dur (F - Barré-Akkord, 1. Bund): E(1), A(3), D(3), G(2), B(1), E2(1)
        {"F", { 1, 3, 3, 2, 1, 1 }},
        // F-Moll (Fm - Barré-Akkord, 1. Bund): E(1), A(3), D(3), G(1), B(1), E2(1)
        {"Fm", { 1, 3, 3, 1, 1, 1 }}
    };
    return map;
}

auto GuitarChords::getFingering(const std::string& chord) const -> Fingering {
    if (const auto it = chordMap.find(chord); it != chordMap.end()) {
        return it->second;
    }

    throw std::out_of_range("Akkord '" + chord + "' nicht gefunden.");
}