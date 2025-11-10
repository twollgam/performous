#include "guitarjsonloader.hh"
#include "json.hh"

#include <fstream>
#include <stdexcept>

namespace {
	void fromJson(const nlohmann::json& j, StrummingPattern& s) {
		s.name = j.value("name", "");
		s.difficulty = j.value("difficulty", 1);
		s.comment = j.value("comment", "");
		//s.accentedBeats = j.value("accented_beats", std::vector<int>{});

		if (j.contains("notation"))
			s.pattern = parseStrokes(j.at("notation").get<std::string>());
	}

	using StrummingLibrary = std::map<std::string, StrummingPatterns>;

	void parseStrummingLibrary(const nlohmann::json& j, StrummingLibrary& library) {
		if (!j.contains("strumming_library"))
			return;

		const auto& libJson = j.at("strumming_library");

		for (auto it = libJson.begin(); it != libJson.end(); ++it) {
			const auto patternKey = it.key();
			const auto& patternArray = it.value();
			auto patterns = StrummingPatterns{ patternKey };

			for (const auto& p : patternArray) {
				auto strummingPattern = StrummingPattern{};

				strummingPattern.name = p["name"].get<std::string>();
				strummingPattern.difficulty = p["difficulty"].get<int>();
				strummingPattern.pattern = parseStrokes(p["notation"].get<std::string>());

				if (p.contains("comment"))
					strummingPattern.comment = p["comment"].get<std::string>();

				patterns.patterns[strummingPattern.difficulty] =strummingPattern;
			}

			library[patternKey] = std::move(patterns);
		}
	}
}

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

	auto strummings = StrummingLibrary{};
	parseStrummingLibrary(j, strummings);

	auto const& sections = j["sections"];
	auto const& structure = j["structure"];

	for (auto const& entry : structure) {
		auto const sectionName = entry["section"].get<std::string>();
		auto const startTime = entry["start_time"].get<double>();
		if (!sections.contains(sectionName))
			continue;
		auto const& section = sections[sectionName];
		if (!section.contains("chords"))
			continue;
		auto const& chords = section["chords"];
		auto const strumming = section.value("strumming", "");

		for (auto const& c : chords) {
			auto const chord = c["chord"].get<std::string>();
			auto const time = c["time"].get<double>();
			auto const duration = c.value("duration", 2.);
			auto& chordTime = provider->addChord(chord, time + startTime + globalGap, duration);

			if (!strumming.empty()) {
				auto const it = strummings.find(strumming);

				if (it != strummings.end()) {
					chordTime.strummings = it->second;
				}
			}
		}

		if (section.contains("repeat")) {
			auto const repeat = section["repeat"].get<int>();
			auto const duration = section.value("duration", 0.0);
			for (int i = 1; i < repeat; ++i) {
				for (auto const& c : chords) {
					auto const chord = c["chord"].get<std::string>();
					auto const chordTime = c["time"].get<double>();
					provider->addChord(chord, chordTime + startTime + (i * duration) + globalGap, duration);
				}
			}
		}
	}

	return provider;
}
