#pragma once

#include "chordsong.hh"
#include "song.hh"

#include <iostream>

class ChordProLoader {
public:
	ChordProLoader(std::istream&, Song const&);

	void load(ChordSong&);

private:
	std::istream& m_stream;
	Song const& m_song;
};
