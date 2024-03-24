#include "guitar_chord_detector.hh"

#include <iostream>
#include <unordered_map>
#include <cmath>

namespace {
    template<class ValueType>
    std::vector<ValueType> getMagnitudes(fft_t const& fftData)
    {
        auto magnitudes = std::vector<ValueType>(fftData.size());

        // Calculate magnitudes squared of frequencies
        for (size_t i = 0; i < fftData.size(); ++i) {
            magnitudes[i] = std::norm(fftData[i]); // Magnitude squared of complex number
        }

        return magnitudes;
    }
}

GuitarChord GuitarChordDetector::detectChord(std::vector<float> const& preprocessedSamples) const {
    // Find the dominant frequency in the spectrum
    auto maxFrequency = 0.0f;
    for (auto sample : preprocessedSamples) {
        if (sample > maxFrequency) {
            maxFrequency = sample;
        }
    }

    // Map dominant frequencies to guitar chords
    auto const chordMap = std::unordered_map<double, GuitarChord>{
        { 261.63, GuitarChord::C },    // C major chord (frequency of middle C)
        { 293.66, GuitarChord::D },    // D major chord
        { 329.63, GuitarChord::E },    // E major chord
        { 349.23, GuitarChord::F },    // F major chord
        { 392.00, GuitarChord::G },    // G major chord
        { 440.00, GuitarChord::A },    // A major chord
        { 493.88, GuitarChord::B }     // B major chord
    };

    auto const it = chordMap.find(maxFrequency);
    if (it != chordMap.end())
        return it->second;

    return GuitarChord::Unknown;
}

GuitarChord GuitarChordDetector::detectChord(fft_t const& fftData) const
{
    return detectChord(getMagnitudes<float>(fftData));
}

ChordVariant GuitarChordDetector::detectChordVariant(std::vector<float> const& frequencySpectrum) const {
    auto const majorRatio = 4.0f / 5.0f; // Major chord: Root, Major 3rd, Perfect 5th
    auto const minorRatio = 10.0f / 12.0f; // Minor chord: Root, Minor 3rd, Perfect 5th
    auto const dominantRatio = 3.0f / 5.0f; // Dominant 7th chord: Root, Major 3rd, Perfect 5th, Minor 7th
    auto const sus2Ratio = 3.0f / 4.0f; // Suspended 2nd chord: Root, Major 2nd, Perfect 5th
    auto const sus4Ratio = 4.0f / 5.0f; // Suspended 4th chord: Root, Perfect 4th, Perfect 5th

    auto hasMajorThird = false;
    auto hasMinorThird = false;
    auto hasPerfectFifth = false;
    auto hasMajorSecond = false;
    auto hasPerfectFourth = false;

    for (double frequency : frequencySpectrum) {
        double normalizedFrequency = frequency / frequencySpectrum[0]; // Normalize to the root frequency

        if (std::abs(normalizedFrequency - majorRatio) < 0.02)
            hasMajorThird = true;

        if (std::abs(normalizedFrequency - minorRatio) < 0.02)
            hasMinorThird = true;

        if (std::abs(normalizedFrequency - 1.5) < 0.02)
            hasPerfectFifth = true;

        if (std::abs(normalizedFrequency - sus2Ratio) < 0.02)
            hasMajorSecond = true;

        if (std::abs(normalizedFrequency - sus4Ratio) < 0.02)
            hasPerfectFourth = true;
    }

    // Determine the chord variant based on the detected intervals
    if (hasMajorThird && hasPerfectFifth) {
        // Determine if it's a dominant 7th chord
        auto hasMinorSeventh = false;
        for (auto frequency : frequencySpectrum) {
            auto normalizedFrequency = frequency / frequencySpectrum[0]; // Normalize to the root frequency
            if (std::abs(normalizedFrequency - dominantRatio) < 0.02) { // Allow some tolerance for frequency matching
                hasMinorSeventh = true;
                break;
            }
        }
        if (hasMinorSeventh)
            return ChordVariant::DominantSeventh;

        return ChordVariant::Major;
    }
    if (hasMinorThird && hasPerfectFifth) {
        // Determine if it's a minor 7th chord
        auto hasMinorSeventh = false;
        for (auto frequency : frequencySpectrum) {
            auto normalizedFrequency = frequency / frequencySpectrum[0]; // Normalize to the root frequency
            if (std::abs(normalizedFrequency - dominantRatio) < 0.02) { // Allow some tolerance for frequency matching
                hasMinorSeventh = true;
                break;
            }
        }
        if (hasMinorSeventh)
            return ChordVariant::MinorSeventh;
        return ChordVariant::Minor;
    }
    if (hasMajorSecond && hasPerfectFifth)
        return ChordVariant::Suspended2;
    if (hasPerfectFourth && hasPerfectFifth)
        return ChordVariant::Suspended4;

    return ChordVariant::Unknown;
}

ChordVariant GuitarChordDetector::detectChordVariant(fft_t const& fftData) const
{
    return detectChordVariant(getMagnitudes<float>(fftData));
}
