#pragma once

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

    // Struktur zur Speicherung der Bund-Informationen für alle 6 Saiten.
    // 0 = Leersaite. -1 oder 99 (hier nicht verwendet) könnte Dämpfen/Mutieren bedeuten.
    struct Fingering {
        int e_low;
        int a;
        int d;
        int g;
        int b;
        int e_high;
    };

    auto getFingering(const std::string& chord) const -> Fingering;

private:
    auto createChordMap() const -> std::map<std::string, Fingering>;
    const std::map<std::string, Fingering> chordMap = createChordMap();
};
