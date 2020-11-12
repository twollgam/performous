#pragma once

#include "playlist.hh"
#include "libxml++-impl.hh"
#include <stdexcept>


/**Exception which will be thrown when loading of PlayLists fails.*/
struct PlayListLoadException: public std::runtime_error {
	PlayListLoadException (std::string const& msg) :
		runtime_error(msg)
	{}
};

class Songs;

class PlayListLoader
{
public:
    PlayListLoader(xmlpp::NodeSet const&, Songs const&);

    void load(PlayLists&) const;

private:
    xmlpp::NodeSet const& m_nodeset;
    Songs const& m_songs;
};
