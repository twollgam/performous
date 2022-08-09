#include "chordsong.hh"

void ChordSong::add(ChordSong::TimePoint timePoint, Chord const& chord) {
	m_chords[timePoint] = chord;
}

ChordSong::ConstIterator ChordSong::begin() const {
	return m_chords.begin();
}

ChordSong::ConstIterator ChordSong::end() const {
	return m_chords.end();
}

ChordSong::ConstIterator ChordSong::get(ChordSong::TimePoint timePointWanted) const {
	if(m_chords.empty())
		return m_chords.end();
	if(timePointWanted < 0.0f)
		return m_chords.end();
	if(timePointWanted > m_chords.rbegin()->first)
		return m_chords.end();

	for(auto it = m_chords.begin(); it != m_chords.end(); ++it) {
		if(timePointWanted == it->first)
			return it;
		if(timePointWanted > it->first)
			return --it;
	}

	return m_chords.end();
}


