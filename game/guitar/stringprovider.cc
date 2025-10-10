#include "stringprovider.hh"

void StringProvider::addChord(std::string const& chord, double const time) {
	std::lock_guard<std::mutex> lock(mutex_);
	chords_.emplace_back(chord, time);
}

ChordTime StringProvider::getChord(double const time) {
	std::lock_guard<std::mutex> lock(mutex_);
	if (chords_.empty() || time < chords_.front().time)
		return { "", 0.0 };
	auto last = chords_.front();
	for (auto const& chord : chords_) {
		if (chord.time <= time) 
			last = chord;
		else 
			break;
	}
	return last;
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
