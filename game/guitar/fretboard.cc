#include "fretboard.hh"

#include "guitar_chords.hh"

#include "graphic/view_trans.hh"

FretBoard::FretBoard(Game& game, Audio& audio, Song const& song, input::DevicePtr dev)
:  InstrumentGraph(game, audio, song, dev), m_game(game),
	m_background(findFile("fretboard.svg")), m_dot(findFile("fretboard_dot.svg")) {

	m_menu.close();
}

void FretBoard::draw(double time) {
	glutil::GLErrorChecker ec("FretBoard::draw");

	//auto& window = m_game.getWindow();
	//ViewTrans view(window, static_cast<float>(m_cx.get()), 0.0f, 0.75f);

	drawBackground();
	drawCurrentFingers(time);
}

void FretBoard::drawCurrentFingers(double time) {
	auto const chords = std::vector<std::string>{
		"Am", "E", "G", "D", "F", "C", "Dm", "E"
	};
	auto const index = int(time / 2.) % chords.size();
	auto const chord = chords[index];
	auto const fingering = GuitarChords().getFingering(chord);

	draw(GuitarChords::GuitarString::E_low, fingering.e_low);
	draw(GuitarChords::GuitarString::A, fingering.a);
	draw(GuitarChords::GuitarString::D, fingering.d);
	draw(GuitarChords::GuitarString::G, fingering.g);
	draw(GuitarChords::GuitarString::B, fingering.b);
	draw(GuitarChords::GuitarString::E_high, fingering.e_high);
}

void FretBoard::draw(GuitarChords::GuitarString string, int fret) {
	if (fret <= 0)
		return;

	auto const wBoard = 0.3f;
	auto const hBoard = 0.075f;
	auto const wBoardOffset = 0.03f;
	auto const hBoardOffset = 0.0225f;
	auto const w = 0.0075f;
	auto const h = 0.0075f;
	auto const top = -0.21f;
	auto const right = 0.45f;
	auto const x = right - (static_cast<float>(fret)) / 6.0f * (wBoard - wBoardOffset);
	auto const y = top + (static_cast<float>(static_cast<int>(string))) / 3.0f * (hBoard - hBoardOffset) - 0.05f;

	UseTexture tex(m_game.getWindow(), m_dot);
	glutil::VertexArray va;
	auto const color = glmath::vec4(1.f, 1.f, 1.f, 1.0f);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 0.0f).vertex(-w + x, -h + y);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 1.0f).vertex(-w + x, h + y);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 0.0f).vertex(w + x, -h + y);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 1.0f).vertex(w + x, h + y);
	va.draw();
}

void FretBoard::drawBackground() {
	UseTexture tex(m_game.getWindow(), m_background);
	glutil::VertexArray va;
	auto const w = 0.3f;
	auto const h = 0.075f;
	auto const top = -0.21f;
	auto const right = 0.45f;
	auto const left = right - w;
	auto const color = glmath::vec4(1.f, 1.f, 1.f, 1.0f);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 0.0f).vertex(-w + left, -h + top);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(0.0f, 1.0f).vertex(-w + left, h + top);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 0.0f).vertex(w + left, -h + top);
	va.normal(0.0f, 1.0f, 0.0f).color(color).texCoord(1.0f, 1.0f).vertex(w + left, h + top);
	va.draw();
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
