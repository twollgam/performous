#pragma once
#include "istringprovider.hh"
#include <vector>
#include <mutex>

class StringProvider : public IGuitarStringProvider {
public:
	StringProvider() = default;

	void addChord(std::string const& chord, double const time);
	ChordTime getChord(double const time) override;
	std::vector<ChordTime> getChords(double const startTime, double const endTime) override;

private:
	std::vector<ChordTime> chords_;
	std::mutex mutex_;
};
