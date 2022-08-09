#pragma once

#include <map>

// dummy
struct Chord{};

class ChordSong {
public:
	using TimePoint = float;
	using Container = std::map<TimePoint, Chord>;
	using ConstIterator = Container::const_iterator;

public:
	void add(TimePoint, Chord const&);

	ConstIterator begin() const;
	ConstIterator end() const;
	ConstIterator get(TimePoint) const;

private:
	std::map<TimePoint, Chord> m_chords;
};
