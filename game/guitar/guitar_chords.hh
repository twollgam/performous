#pragma once

#include "fingering.hh"

#include <string>
#include <map>

class GuitarChords {
public:
    enum class GuitarString {
        E_low,
        A,
        D,
        G,
        B,
        E_high
    };

    auto getFingering(const std::string& chord) const -> Fingering;

private:
    auto createChordMap() const -> std::map<std::string, Fingering>;
    const std::map<std::string, Fingering> chordMap = createChordMap();
};
