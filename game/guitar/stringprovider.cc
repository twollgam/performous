#include "stringprovider.hh"

ChordTime& StringProvider::addChord(std::string const& chord, double const time, double duration) {
	std::lock_guard<std::mutex> lock(mutex_);
	return chords_.emplace_back(chord, time, duration);
}

ChordTime StringProvider::getChord(double const time) {
	std::lock_guard<std::mutex> lock(mutex_);

	if (chords_.empty() || time < chords_.front().time)
		return { "", 0.0 };

	for (auto const& chord : chords_) {
		if (time >= chord.time && time <= chord.time + chord.duration) 
			return chord;
	}

	return { "", 0.0 };
}

std::vector<ChordTime> StringProvider::getChords(double const startTime, double const endTime) {
	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<ChordTime> result;

	for (auto const& chord : chords_) {
		if (chord.time >= startTime && chord.time <= endTime)
			result.push_back(chord);
	}

	return result;
}
