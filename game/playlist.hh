
#pragma once

#include "song.hh"
#include <memory>
#include <mutex>
#include <vector>

class PlayList
{
public:
	//needs function that returns the song properties in a list for display on screen

	using SongList = std::vector<std::shared_ptr<Song>>;

	PlayList() = default;
	PlayList(PlayList const&);
	PlayList(PlayList&&) = default;

	/// Adds a new song to the queue
	void addSong(std::shared_ptr<Song> song);
	/// Returns the next song and removes it from the queue
	std::shared_ptr<Song> getNext();
	/// Returns all currently queued songs
	SongList& getList();
	const SongList& getList() const;
	///array-access should replace getList!!
	std::shared_ptr<Song> operator[](std::size_t index) { return m_list[index]; }
	/// Returns true if the queue is empty
	bool isEmpty();
	/// Randomizes the order of the playlist
	void shuffle();
	///clears list
	void clear();
	///removes a song
	void removeSong(int index);
	/// swaps two songs
	void swap (int index1, int index2);
	void setPosition (unsigned int index1, unsigned int index2);
	/// gets a specific song and removes it from the queue
	std::shared_ptr<Song> getSong(int index);
	/// this is for the webserver, to avoid crashing when adding the current playing song
	std::shared_ptr<Song> currentlyActive;

	const std::string& getName() const;
	void setName(std::string const &);

private:
	SongList m_list;
	std::string m_name;
	mutable std::unique_ptr<std::mutex> m_mutex{std::make_unique<std::mutex>()};
};

using PlayListPtr = std::shared_ptr<PlayList>;
using PlayLists = std::vector<PlayListPtr>;
