#pragma once

#include "fft.hh"

#include <vector>

enum class GuitarChord {
    C,
    D,
    E,
    F,
    G,
    A,
    B,
    Unknown
};

enum class ChordVariant {
    Major,
    Minor,
    MajorSeventh,
    MinorSeventh,
    DominantSeventh,
    Suspended2,
    Suspended4,
    Unknown
};

class GuitarChordDetector {
public:
    GuitarChord detectChord(std::vector<float> const& preprocessedSamples) const;
    GuitarChord detectChord(fft_t const&) const;
    ChordVariant detectChordVariant(std::vector<float> const& frequencySpectrum) const;
    ChordVariant detectChordVariant(fft_t const&) const;
};
