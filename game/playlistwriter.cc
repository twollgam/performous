#include "playlistwriter.hh"

#include "screen.hh"
#include "songs.hh"

PlayListWriter::PlayListWriter(xmlpp::Element& element, Songs const& songs)
: m_element(element), m_songs(songs)
{
}

void PlayListWriter::save(PlayLists const& lists) const
{    
    auto const& songs = m_songs.getLoadedSongs();

	for (auto const& list: lists) {
		auto* playlist = xmlpp::add_child_element(&m_element, "playlist");

		playlist->set_attribute("name", list->getName());

        for(auto const& song : list->getList()) {
			auto* songElement = xmlpp::add_child_element(playlist, "song");

			songElement->set_attribute("title", song->title);
			songElement->set_attribute("artist", song->artist);
		}
	}
}
