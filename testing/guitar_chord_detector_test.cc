#include "common.hh"

#include "game/guitar/guitar_chord_detector.hh"


class UnitTest_GuitarChordDetector : public ::testing::Test {
protected:
    void SetUp() override {
    }

    void TearDown() override {
    }
};

TEST_F(UnitTest_GuitarChordDetector, detectChord_E) {
    std::vector<float> frequencySpectrum = { /* Add frequency spectrum for a major chord */ };
    EXPECT_EQ(GuitarChord::E, GuitarChordDetector().detectChord(frequencySpectrum));
}

TEST_F(UnitTest_GuitarChordDetector, detectChord_F) {
    std::vector<float> frequencySpectrum = { /* Add frequency spectrum for a minor chord */ };
    EXPECT_EQ(GuitarChord::F, GuitarChordDetector().detectChord(frequencySpectrum));
}

TEST_F(UnitTest_GuitarChordDetector, detectChordVariant_MajorChord) {
    std::vector<float> frequencySpectrum = { /* Add frequency spectrum for a major chord */ };
    EXPECT_EQ(ChordVariant::Major, GuitarChordDetector().detectChordVariant(frequencySpectrum));
}

TEST_F(UnitTest_GuitarChordDetector, detectChordVariant_MinorChord) {
    std::vector<float> frequencySpectrum = { /* Add frequency spectrum for a minor chord */ };
    EXPECT_EQ(ChordVariant::Minor, GuitarChordDetector().detectChordVariant(frequencySpectrum));
}

