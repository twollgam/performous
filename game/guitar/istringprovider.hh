#pragma once

#include <memory>

struct ChordTime {
	std::string chord;
	double time;

	ChordTime(const std::string& chord, double time)
		: chord(chord), time(time) {
	}
};

struct IGuitarStringProvider {
	virtual ~IGuitarStringProvider() = default;

	virtual ChordTime getChord(double time) = 0;
};

using GuitarStringProvider = std::shared_ptr<IGuitarStringProvider>;
