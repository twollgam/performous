#include "guitarjsonloader.hh"
#include "json.h"
#include <fstream>
#include <stdexcept>

std::shared_ptr<StringProvider> GuitarJsonLoader::load(std::string const& path) {
	std::ifstream in(path);

	if (!in.is_open())
		throw std::runtime_error("Cannot open file: " + path);

	nlohmann::json j;
	in >> j;
	in.close();

	auto provider = std::make_shared<StringProvider>();

	double globalGap = 0.0;
	if (j.contains("meta") && j["meta"].contains("time_gap"))
		globalGap = j["meta"]["time_gap"].get<double>();

	if (!j.contains("sections") || !j.contains("structure"))
		throw std::runtime_error("Invalid JSON: missing 'sections' or 'structure'");

	auto const& sections = j["sections"];
	auto const& structure = j["structure"];

	for (auto const& entry : structure) {
		auto const sectionName = entry["section"].get<std::string>();
		auto const startTime = entry["start_time"].get<double>();
		if (!sections.contains(sectionName)) continue;
		auto const& section = sections[sectionName];
		if (!section.contains("chords")) continue;
		auto const& chords = section["chords"];

		for (auto const& c : chords) {
			auto const chord = c["chord"].get<std::string>();
			auto const chordTime = c["time"].get<double>();
			provider->addChord(chord, chordTime + startTime + globalGap);
		}

		if (section.contains("repeat")) {
			auto const repeat = section["repeat"].get<int>();
			auto const duration = section.value("duration", 0.0);
			for (int i = 1; i < repeat; ++i) {
				for (auto const& c : chords) {
					auto const chord = c["chord"].get<std::string>();
					auto const chordTime = c["time"].get<double>();
					provider->addChord(chord, chordTime + startTime + (i * duration) + globalGap);
				}
			}
		}
	}

	return provider;
}
