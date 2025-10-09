#include "fretboard.hh"

#include "guitar_chords.hh"

#include "graphic/view_trans.hh"
#include "graphic/text_renderer.hh"

FretBoard::FretBoard(Game& game, Audio& audio, Song const& song, input::DevicePtr dev, GuitarStringProvider provider)
:  InstrumentGraph(game, audio, song, dev), m_game(game), m_provider(provider),
	m_background(findFile("fretboard.svg")), m_dot(findFile("fretboard_dot.svg")), m_muted(findFile("muted_string.svg")), 
	m_layout(parseLayout(findFile("fretboard.json"))) {

	m_menu.close();
}

void FretBoard::draw(double time) {
	glutil::GLErrorChecker ec("FretBoard::draw");

	//auto& window = m_game.getWindow();
	//ViewTrans view(window, static_cast<float>(m_cx.get()), 0.0f, 0.75f);

	drawBackground();
	drawCurrentFingers(time);
	drawChord(time);
}

void FretBoard::drawChord(double time) {
	auto const chords = m_provider->getChords(time, time + 2.0);

	if (chords.empty())
		return;

	auto const top = m_top + m_layout.top * m_boardHeight;
	auto const bottom = m_top + m_boardHeight -  m_layout.bottom * m_boardHeight;
	auto const center = (top + bottom) * 0.5f;
	auto const right = m_right;
	auto style = TextStyle();
	style.fontalign = "center";
	style.fontweight = "bold";
	style.fontstyle = "normal";
	style.stroke_col = Color(0.f, 0.f, 0.f);
	style.fill_col = Color(0.4f, 0.5f, 0.9f, 0.9f);
	//style.stroke_width = 0.5f;

	m_chord = std::make_unique<OpenGLText>(TextRenderer().render(chords[0].chord, style, 1.0));

	m_chord->dimensions().fixedWidth(0.04f).fixedHeight(0.04f).center(center).left(right);
	m_chord->draw(m_game.getWindow());

	if (chords.size() < 2)
		return;

	m_chordNext = std::make_unique<OpenGLText>(TextRenderer().render(chords[1].chord, style, 1.0));

	m_chordNext->dimensions().fixedWidth(0.02f).fixedHeight(0.02f).center(center).left(right + 0.05f);
	m_chordNext->draw(m_game.getWindow());
}

void FretBoard::drawCurrentFingers(double time) {
	auto const chord = m_provider->getChord(time);
	auto const defaultColor = glmath::vec4{ 0.f, 0.f, 0.f, 1.f };

	if (!chord.chord.empty()) {
		auto const fingering = GuitarChords().getFingering(chord.chord);

		draw(GuitarChords::GuitarString::E_low, fingering.e_low, 1.f, defaultColor);
		draw(GuitarChords::GuitarString::A, fingering.a, 1.f, defaultColor);
		draw(GuitarChords::GuitarString::D, fingering.d, 1.f, defaultColor);
		draw(GuitarChords::GuitarString::G, fingering.g, 1.f, defaultColor);
		draw(GuitarChords::GuitarString::B, fingering.b, 1.f, defaultColor);
		draw(GuitarChords::GuitarString::E_high, fingering.e_high, 1.f, defaultColor);
	}

	auto const nextChord = m_provider->getChord(time + 2.);

	if (!nextChord.chord.empty()) {
		auto const fingering = GuitarChords().getFingering(nextChord.chord);
		auto const opacity = 1.0 - (nextChord.time - time) * 0.5;
		auto const color = glmath::vec4{0.3f, 0.f, 0.5f, 0.f} * float(1.0 - opacity) + defaultColor * float(opacity);
		auto const radius = opacity;
		draw(GuitarChords::GuitarString::E_low, fingering.e_low, radius, color);
		draw(GuitarChords::GuitarString::A, fingering.a, radius, color);
		draw(GuitarChords::GuitarString::D, fingering.d, radius, color);
		draw(GuitarChords::GuitarString::G, fingering.g, radius, color);
		draw(GuitarChords::GuitarString::B, fingering.b, radius, color);
		draw(GuitarChords::GuitarString::E_high, fingering.e_high, radius, color);
	}
}

void FretBoard::draw(GuitarChords::GuitarString string, int fret, float radius, glmath::vec4 color) {
	if (fret == 0)
		return;

	auto const wBoard = m_boardWidth - (m_layout.left + m_layout.right) * m_boardWidth;
	auto const hBoard = m_boardHeight - (m_layout.top + m_layout.bottom) * m_boardHeight;
	auto const w = 0.0075f * radius;
	auto const h = 0.0075f * radius;
	auto const top = m_top + m_layout.top * m_boardHeight;
	auto const right = m_right - m_layout.right * m_boardWidth;
	auto const y = top + (static_cast<float>(static_cast<int>(string)) / 5.0f) * hBoard;

	if (fret < 0) {
		auto const x = right + 0.05f * wBoard;
		UseTexture tex(m_game.getWindow(), m_muted);
		glutil::VertexArray va;
		va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 0.0f).vertex(-w + x, -h + y);
		va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 1.0f).vertex(-w + x, h + y);
		va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 0.0f).vertex(w + x, -h + y);
		va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 1.0f).vertex(w + x, h + y);
		va.draw();

		return;
	}

	auto const x = right - ((static_cast<float>(fret) - 0.5f) / static_cast<float>(m_layout.frets)) * wBoard;

	UseTexture tex(m_game.getWindow(), m_dot);
	glutil::VertexArray va;
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 0.0f).vertex(-w + x, -h + y);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 1.0f).vertex(-w + x, h + y);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 0.0f).vertex(w + x, -h + y);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 1.0f).vertex(w + x, h + y);
	va.draw();
}

void FretBoard::drawBackground() {
	UseTexture tex(m_game.getWindow(), m_background);
	glutil::VertexArray va;
	auto const w2 = m_boardWidth * 0.5f;
	auto const h2 = m_boardHeight * 0.5f;
	auto const top = m_top;
	auto const right = m_right;
	auto const cx = right - w2;
	auto const cy = top + h2;
	auto const color = glmath::vec4(1.f, 1.f, 1.f, 1.0f);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 0.0f).vertex(-w2 + cx, -h2 + cy);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 1.0f).vertex(-w2 + cx, h2 + cy);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 0.0f).vertex(w2 + cx, -h2 + cy);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 1.0f).vertex(w2 + cx, h2 + cy);
	va.draw();
}

FretBoard::Layout FretBoard::parseLayout(const std::filesystem::path& path) {
	auto layout = Layout();
	const auto data = readJSON(path);

	if (data.is_object()) {
		if (data.contains("top"))
			layout.top = data["top"].get<float>();
		if (data.contains("left"))
			layout.left = data["left"].get<float>();
		if (data.contains("right"))
			layout.right = data["right"].get<float>();
		if (data.contains("bottom"))
			layout.bottom = data["bottom"].get<float>();
		if (data.contains("linear"))
			layout.linear = data["linear"].get<bool>();
		if (data.contains("frets"))
			layout.frets = data["frets"].get<int>();
	}

	return layout;
}

void FretBoard::engine() {
}

std::string FretBoard::getTrack() const {
    return "RealGuitar";
}

std::string FretBoard::getDifficultyString() const {
    return {};
}

std::string FretBoard::getModeId() const {
	return {};
}

void FretBoard::changeTrack(int) {
}

void FretBoard::changeDifficulty(int) {
}

bool FretBoard::ignoreLayout() const {
	return true;
}

bool FretBoard::modifyAudioTrack(AudioModification& volume, AudioModification& pitchbend) {
	return false;
}
