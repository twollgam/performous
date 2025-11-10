#pragma once

// No shaders, no textures — ideal for testing and prototyping.

#include "irenderer.hh"
#include "note.hh"

#include "opengl_text.hh"
#include "graphic/texture_grid.hh"

class Game;

class SimpleGLRenderer : public Guitar::IRenderer {
public:
    SimpleGLRenderer(Game&);
    ~SimpleGLRenderer() override = default;

    void init() override;
    void drawFretboard(const Guitar::Note& note, const Guitar::RenderParams& params) override;
    void drawNotes(double time, const std::vector<Guitar::Note>& note, const Guitar::RenderParams& params) override;
    void drawOverlayText(const std::string& text, float x, float y) override;
    void present() override;

private:
    void drawMarker(int fret, int string, const Guitar::RenderParams&);
    void drawMarker(float x, float y, float w, float h);
    void drawFingering(float xOffset, float yOffset, int string, int fret, float opacity, const Guitar::RenderParams&);
    void drawStrings(const Guitar::Note& note, const Guitar::RenderParams& params);
    void drawFrets(const Guitar::RenderParams& params);
    void drawMarkers(const Guitar::RenderParams& params);
    void drawNote(double time, const Guitar::Note& note, const Guitar::RenderParams& params);
    void drawNoteBlock(double time, const Guitar::Note& note, const Guitar::RenderParams& params);
    void drawNoteBlockStroke(double time, const Guitar::Note& note, float opacity, const Guitar::RenderParams& params);
    void drawOverlayText(const std::string& text, float x, float y, float opacity);

private:
    Game& m_game;
    Texture m_line;
    Texture m_marker;
    Texture m_stroke;
    TextureGrid m_strokeGrid;
    std::unique_ptr<OpenGLText> m_chord;
    const float m_boardWidth = 0.8f;
    const float m_boardHeight = 0.1f;
    const float m_boardXOffset = -0.4f;
    const float m_boardYOffset = 0.15f;
};

