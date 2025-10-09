#include "stringprovider.hh"

void StringProvider::addChord(std::string const& chord, double const time) {
	std::lock_guard<std::mutex> lock(mutex_);
	chords_.emplace_back(chord, time);
}

ChordTime StringProvider::getChord(double const time) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (chords_.empty()) return { "", 0.0 };
	auto last = chords_.front();
	for (auto const& c : chords_) {
		if (c.time <= time) last = c;
		else break;
	}
	return last;
}

std::vector<ChordTime> StringProvider::getChords(double const startTime, double const endTime) {
	std::lock_guard<std::mutex> lock(mutex_);
	std::vector<ChordTime> result;
	for (auto const& c : chords_) {
		if (c.time >= startTime && c.time <= endTime) result.push_back(c);
	}
	return result;
}
