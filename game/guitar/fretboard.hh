#pragma once

#include "instrumentgraph.hh"
#include "guitar_chords.hh"
#include "istringprovider.hh"


struct TestStringProvider : public IGuitarStringProvider {
    ~TestStringProvider() override = default;

    ChordTime getChord(double time) override {
        if (time < 0)
            return {"", 0.};

        auto const index = int(time / 2.) % chords.size();
        auto const chord = chords[index];

        return { chord, floor(time / 2.) * 2.};
    }
    std::vector<ChordTime> getChords(double startTime, double endTime) override {
        if (endTime < 0)
            return {};

        auto const startIndex = startTime < 0. ? int((startTime + 2.) / 2.) - 1 : int(startTime / 2.);
        auto const endIndex = int(endTime / 2.);
        auto result = std::vector<ChordTime>{};

        for (auto index = startIndex; index <= endIndex; ++index) {
            auto const f = float(index - startIndex) / float(endIndex - startIndex);
            auto const time = endTime * f + startTime * (1.f - f);

            if (index < 0) {
                result.emplace_back("", floor(time / 2.) * 2.);

                continue;
            }

            auto const wrappedIndex = index % chords.size();
            auto const chord = chords[wrappedIndex];

            result.emplace_back(chord, floor(time / 2.) * 2.);
        }

        return result;
    }

private:
    std::vector<std::string> const chords {
        "Am", "E", "G", "D", "F", "C", "Dm", "E",
        "Am", "E", "G", "D", "F", "C", "Dm", "E",
        "F", "C", "E", "Am", "F", "C", "Dm", "E",
        "Am", "E", "G", "D", "F", "C", "Dm", "E",
        "Am", "E", "G", "D", "F", "C", "Dm", "E",
        "F", "C", "E", "Am", "F", "C", "Dm", "E"
    };

};

class FretBoard : public InstrumentGraph {
public:
    FretBoard(Game& game, Audio& audio, Song const& song, input::DevicePtr dev, GuitarStringProvider = std::make_shared<TestStringProvider>());

    void draw(double time) override;
    void engine() override;
    std::string getTrack() const override;
    std::string getDifficultyString() const override;
    std::string getModeId() const override;
    void changeTrack(int dir) override;
    void changeDifficulty(int dir) override;

    bool ignoreLayout() const override;
    bool modifyAudioTrack(AudioModification& volume, AudioModification& pitchbend) override;

private:
    struct Layout {
        float top = 0.f;
        float left = 0.f;
        float right = 0.f;
        float bottom = 0.f;
        bool linear = true;
        int frets = 12;
    };

    void drawCurrentFingers(double time);
    void draw(GuitarChords::GuitarString string, int fret, float radius = 1.f, glmath::vec4 color = {1.f, 1.f, 1.f, 1.f});
    void drawChord(double time);
    void drawBackground();
    static Layout parseLayout(const std::filesystem::path&);

    Game& m_game;
    Texture m_background;
    Texture m_dot;
    Texture m_muted;
    std::unique_ptr<OpenGLText> m_chord;
    std::unique_ptr<OpenGLText> m_chordNext;
    Layout m_layout;
    float const m_boardWidth = 0.5f;
    float const m_boardHeight = 0.125f;
    float const m_top = -0.25f;
    float const m_right = 0.4f;

};
