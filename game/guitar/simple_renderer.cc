#include "simple_renderer.hh"

#include "graphic/view_trans.hh"
#include "graphic/text_renderer.hh"
#include "graphic/texture_grid_loader.hh"

#include "game.hh"


SimpleGLRenderer::SimpleGLRenderer(Game& game)
    : m_game(game),
    m_line(findFile("white_square.svg")),
    m_marker(findFile("fretboard_marker.svg")),
    m_stroke(findFile("stroke_up.svg")) {
    try {
        m_strokeGrid = TextureGridLoader().loadFromJson(findFile("stroke_up.json"));
    }
    catch (const std::exception&) {
    }
}

void SimpleGLRenderer::init() {
}

void SimpleGLRenderer::drawFretboard(const Guitar::Note& note, const Guitar::RenderParams& params) {
    drawStrings(note, params);
    drawFrets(params);
    drawMarkers(params);
}

void SimpleGLRenderer::drawStrings(const Guitar::Note& note, const Guitar::RenderParams& params) {
    auto const h = 0.001f;
    UseTexture tex(m_game.getWindow(), m_line);

    for (int s = 0; s < 6; ++s) {
        auto const color = params.stringColors[s] * (note.fingering[s] == -1 ? 0.3f : 1.f);
        auto const x = m_boardXOffset;
        auto const y = m_boardYOffset + float(s) / 5.f * 0.1f;

        glutil::VertexArray va;
        va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 0.0f).vertex(x, -h + y);
        va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 1.0f).vertex(x, h + y);
        va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 0.0f).vertex(x + m_boardWidth, -h + y);
        va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 1.0f).vertex(x + m_boardWidth, h + y);
        va.draw();
    }
}

void SimpleGLRenderer::drawFrets(const Guitar::RenderParams& params) {
    UseTexture tex(m_game.getWindow(), m_line);

    auto const fretColor = glmath::vec4{ 0.3f, 0.3f, 0.3f, 1.f };
    for (int f = 0; f <= params.visibleFrets; ++f) {
        auto const x = m_boardXOffset + float(f) / float(params.visibleFrets) * m_boardWidth;
        auto const top = m_boardYOffset - 0.01f;
        auto const bottom = top + m_boardHeight + 0.02f;
        auto const h = f == 0 ? 0.002f : 0.001f;

        glutil::VertexArray va;
        va.normal(0.0f, 1.0f, 0.0f).color(fretColor).texCoord(0.0f, 0.0f).vertex(-h + x, top);
        va.normal(0.0f, 1.0f, 0.0f).color(fretColor).texCoord(0.0f, 1.0f).vertex(-h + x, bottom);
        va.normal(0.0f, 1.0f, 0.0f).color(fretColor).texCoord(1.0f, 0.0f).vertex(h + x, top);
        va.normal(0.0f, 1.0f, 0.0f).color(fretColor).texCoord(1.0f, 1.0f).vertex(h + x, bottom);
        va.draw();
    }
}

void SimpleGLRenderer::drawMarkers(const Guitar::RenderParams& params) {
    drawMarker(2, 0, params);
    drawMarker(4, 0, params);
    drawMarker(6, 0, params);
    drawMarker(8, 0, params);
    drawMarker(11, -2, params);
    drawMarker(11, 2, params);
    drawMarker(14, 0, params);
    drawMarker(16, 0, params);
    drawMarker(18, 0, params);
    drawMarker(20, 0, params);
}

void SimpleGLRenderer::drawMarker(int fret, int string, const Guitar::RenderParams& params) {
    if (fret >= params.visibleFrets)
        return;

    auto const x = m_boardXOffset + (float(fret) + 0.5f) / float(params.visibleFrets) * m_boardWidth;
    auto const y = m_boardYOffset + 0.05f + float(string) / 5.f * 0.1f;
    auto const l = 0.1f / 12.f;

    drawMarker(x, y, l, l);
}

void SimpleGLRenderer::drawMarker(float x, float y, float w, float h) {
    auto const markerColor = glmath::vec4{ 0.3f, 0.3f, 0.3f, 1.f };
    UseTexture marker(m_game.getWindow(), m_marker);

    glutil::VertexArray va;
    va.normal(0.0f, 1.0f, 0.0f).color(markerColor).texCoord(0.0f, 0.0f).vertex(-w + x, -h + y);
    va.normal(0.0f, 1.0f, 0.0f).color(markerColor).texCoord(0.0f, 1.0f).vertex(-w + x, h + y);
    va.normal(0.0f, 1.0f, 0.0f).color(markerColor).texCoord(1.0f, 0.0f).vertex(w + x, -h + y);
    va.normal(0.0f, 1.0f, 0.0f).color(markerColor).texCoord(1.0f, 1.0f).vertex(w + x, h + y);
    va.draw();
}

void SimpleGLRenderer::drawNotes(double time, const std::vector<Guitar::Note>& notes, const Guitar::RenderParams& params) {
    if (notes.empty())
        return;

    const auto hitWindow = 0.1;
    auto drawnCurrent = false;

    for (const auto& note : notes) {
        const auto delta = note.time - time;

        if (!drawnCurrent && std::abs(delta) < hitWindow) {
            drawNote(time, note, params);
            drawnCurrent = true;
        }
        else if (delta > -hitWindow) {
            drawNoteBlock(time, note, params);
        }
    }
}

void SimpleGLRenderer::drawNoteBlock(double time, const Guitar::Note& note, const Guitar::RenderParams& params) {
    UseTexture tex(m_game.getWindow(), m_line);

    double deltaTime = note.time - time;

    if (deltaTime < 0.0)
        return;

    const auto startX = m_boardXOffset + 0.2f;
    const auto targetX = m_boardXOffset;
    const auto startY = m_boardYOffset - 0.1f;
    const auto targetY = m_boardYOffset;
    const auto f = float(deltaTime) / 10.f;
    const auto opacity = std::max(1.f - f, 0.f);
    const auto xOffset = startX * f + targetX * (1.f - f);
    const auto y = startY * f + targetY * (1.f - f);

    std::vector<std::pair<int, int>> activeStrings;
    activeStrings.reserve(6);

    auto checkString = [&](int idx, int fret) {
        if (fret >= 0)
            activeStrings.emplace_back(idx, fret);
        };
    for (auto string = 0; string < 6; ++string)
        checkString(string, note.fingering[string]);

    if (activeStrings.empty())
        return;

    auto minFret = 1;
    for (auto& [string, fret] : activeStrings)
        minFret = std::min(minFret, std::max(fret, 1));

    const auto blockFrets = 4;
    const auto blockWidth = (m_boardWidth / float(params.visibleFrets)) * blockFrets;
    const auto x = xOffset + float(minFret - 1) / float(params.visibleFrets) * m_boardWidth;
    const auto blockHeight = m_boardHeight * 1.1f;
    const auto borderYOffset = -m_boardHeight * 0.1f * 0.5f;
    const auto borderThickness = 0.002f;
    const auto borderColor = glmath::vec4{ 0.1f, 0.2f, 0.6f, 0.7f * opacity };

    auto drawQuad = [&](float cx, float cy, float w, float h, const glmath::vec4& color) {
        glutil::VertexArray va;
        va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 0.0f).vertex(cx, cy);
        va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 1.0f).vertex(cx, cy + h);
        va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 0.0f).vertex(cx + w, cy);
        va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 1.0f).vertex(cx + w, cy + h);
        va.draw();
        };
    auto drawBox = [&](float x, float y, float w, float h, float thickness, const glmath::vec4& color) {
        drawQuad(x - thickness, y - thickness, w + thickness, thickness, color);
        drawQuad(x - thickness, y, thickness, h, color);
        drawQuad(x - thickness, y + h, w + thickness, thickness, color);
        drawQuad(x + w, y, thickness, h, color);
        };

    drawBox(x, y + borderYOffset, blockWidth, blockHeight, borderThickness, borderColor);

    for (auto& [string, fret] : activeStrings) {
        auto const color = params.stringColors[string] * opacity;
        auto const stringY = y + float(string) / 5.f * m_boardHeight;
        drawQuad(x, stringY, blockWidth, 0.002f, color);
    }

    for (auto string = 0; string < 6; ++string)
        drawFingering(x, y, string, note.fingering[string], opacity, params);

    drawOverlayText(note.chord, x - 0.01f, y, opacity);

    auto const strokeWidth = blockWidth * 0.25f;
    auto const strokeHight = blockHeight * 1.f;
    const auto strokeColor = glmath::vec4{ 1.0f, 1.0f, 1.0f, opacity };
    m_strokeGrid.draw(m_stroke, m_game, x - strokeWidth, y, strokeWidth, strokeHight, strokeWidth, strokeWidth, strokeColor);
}

void SimpleGLRenderer::drawNoteBlockStroke(double time, const Guitar::Note& note, float opacity, const Guitar::RenderParams& params) {
}

void SimpleGLRenderer::drawNote(double time, const Guitar::Note& note, const Guitar::RenderParams& params) {
    for (auto string = 0; string < 6; ++string)
        drawFingering(m_boardXOffset, m_boardYOffset, string, note.fingering[string], 1.f, params);
}

void SimpleGLRenderer::drawFingering(float xOffset, float yOffset, int string, int fret, float opacity, const Guitar::RenderParams& params) {
    if (fret <= 0)
        return;

    auto const color = params.stringColors[string] * opacity;
    auto const halfWidth = m_boardWidth * 0.5f;
    auto const x = xOffset + (float(fret) - 0.5f) / float(params.visibleFrets) * m_boardWidth;
    auto const y = yOffset + float(string) / 5.f * m_boardHeight;
    auto const w = m_boardWidth / float(params.visibleFrets) * 0.25f;
    auto const h = m_boardHeight / 6.f * 0.25f;

    UseTexture tex(m_game.getWindow(), m_line);
    glutil::VertexArray va;
    va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 0.0f).vertex(-w + x, -h + y);
    va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 1.0f).vertex(-w + x, h + y);
    va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 0.0f).vertex(w + x, -h + y);
    va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 1.0f).vertex(w + x, h + y);
    va.draw();
}

void SimpleGLRenderer::drawOverlayText(const std::string& text, float x, float y) {
    drawOverlayText(text, x, y, 1.f);
}

void SimpleGLRenderer::drawOverlayText(const std::string& text, float x, float y, float opacity) {
    auto style = TextStyle();
    style.fontalign = "center";
    style.fontweight = "bold";
    style.fontstyle = "normal";
    style.stroke_col = Color(0.f, 0.f, 0.f, opacity);
    style.fill_col = Color(0.4f, 0.5f, 0.9f, 0.9f * opacity);
    //style.stroke_width = 0.5f;

    m_chord = std::make_unique<OpenGLText>(TextRenderer().render(text, style, 1.0));

    m_chord->dimensions().fixedWidth(0.04f).fixedHeight(0.04f).center(y).right(x);
    m_chord->draw(m_game.getWindow());
}

void SimpleGLRenderer::present() {
    //glFlush(); // let Performous swap buffers
}
