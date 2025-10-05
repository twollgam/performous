#pragma once

#include "instrumentgraph.hh"
#include "guitar_chords.hh"

class FretBoard : public InstrumentGraph {
public:
    FretBoard(Game& game, Audio& audio, Song const& song, input::DevicePtr dev);

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
    void drawCurrentFingers(double time);
    void draw(GuitarChords::GuitarString string, int fret);
    void drawBackground();

    Game& m_game;
    Texture m_background;
    Texture m_dot;
};
