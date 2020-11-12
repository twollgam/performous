#pragma once

#include "playlist.hh"
#include "libxml++-impl.hh"
#include <stdexcept>


/**Exception which will be thrown when saving PlayLists fails.*/
struct PlayListWriteException: public std::runtime_error {
	PlayListWriteException (std::string const& msg) :
		runtime_error(msg)
	{}
};

class Songs;

class PlayListWriter
{
public:
    PlayListWriter(xmlpp::Element&, Songs const&);

    void save(PlayLists const&) const;

private:
    xmlpp::Element& m_element;
    Songs const& m_songs;
};
