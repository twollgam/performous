#pragma once

#include "strumming_pattern.hh"

#include <string>
#include <memory>
#include <vector>

struct ChordTime {
	std::string chord;
	StrummingPatterns strummings;
	double time;
	double duration;

	ChordTime(const std::string& chord, double time, double duration = 4.)
		: chord(chord), time(time), duration(duration) {
	}
};

struct IGuitarStringProvider {
	virtual ~IGuitarStringProvider() = default;

	virtual ChordTime getChord(double time) = 0;
	virtual std::vector<ChordTime> getChords(double startTime, double endTime) = 0;
};

using GuitarStringProvider = std::shared_ptr<IGuitarStringProvider>;
