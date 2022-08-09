#include "chordproloader.hh"

#include <string>
#include <vector>

ChordProLoader::ChordProLoader(std::istream& stream, Song const& song)
:  m_stream(stream), m_song(song) {
}

namespace {
	std::vector<std::string> loadText(std::istream& stream) {
		auto result = std::vector<std::string>();
		auto line = std::string();

		while(std::getline(stream, line))
			result.emplace_back(line);

		return result;
	}
	std::vector<std::string> getChorus(std::vector<std::string> const& text) {
		auto result = std::vector<std::string>();
		auto chorus = false;

		for(auto const& line : text) {
			if(line == "{end_of_chorus}")
				chorus = false;
			if(chorus)
				result.emplace_back(line);
			if(line == "{start_of_chorus}")
				chorus = true;
		}

		return result;
	}
	std::vector<std::string> filter(std::vector<std::string> const& text) {
		auto result = std::vector<std::string>();

		for(auto line : text) {
			auto start = line.find('[');

			while(start != std::string::npos) {
				auto const end = line.find(']');

				if(end != std::string::npos)
					line.erase(start, end - start);
				else
					start = end;
			}

			result.emplace_back(line);
		}

		return result;
	}

	using Block = std::pair<Notes::const_iterator, Notes::const_iterator>;

	Block findBlock(Notes::const_iterator start, Notes::const_iterator end, std::vector<std::string> const& text) {
		auto const barText = filter(text);
		auto position = start;

		for(;position != end; ++position) {
			auto const& syllable = position->syllable;

			if(syllable.empty())
				continue;
			if(barText.at(0).substr(0, syllable.size()) != syllable)
				continue;

			auto textIndex = 0U;
			auto lineIndex = 0U;
			for(auto it = position; it != end; ++it) {
				if(barText.at(lineIndex).substr(textIndex, it->syllable.size()) != it->syllable)
					break;

				textIndex += it->syllable.size();

				if(textIndex >= barText.at(lineIndex).size()) {
					textIndex = 0;
					if(++lineIndex >= barText.size())
						return std::make_pair(position, ++it);
				}
			}

			return std::make_pair(end, end);
		}
	}
	void addBlock(ChordSong& song, Block const& block, std::vector<std::string> const& text) {

	}
}

void ChordProLoader::load(ChordSong& song) {
	auto const fulltext = loadText(m_stream);
	auto const chorus = getChorus(fulltext);
	auto const& track = m_song.vocalTracks.at(0);
	auto const block = findBlock(track.notes.begin(), track.notes.end(), chorus);

	if(block.first != block.second)
		addBlock(song, block, chorus);
}
