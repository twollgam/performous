#include "playlistloader.hh"

#include "screen.hh"
#include "songs.hh"

PlayListLoader::PlayListLoader(xmlpp::NodeSet const& nodeset, Songs const& songs)
: m_nodeset(nodeset), m_songs(songs)
{
}

namespace {
    std::shared_ptr<Song> find(SongVector const& songs, std::string const& title, std::string const& artist) {
        auto it = std::find_if(songs.begin(), songs.end(), 
            [title, artist](const auto& song){ return song->title == title && song->artist == artist;});
    
        if(it == songs.end())
            it = std::find_if(songs.begin(), songs.end(), 
                [title](const auto& song){ return song->title == title;});

        if(it == songs.end())
            return {};

        return *it;
    }

    std::string getAttributeValue(xmlpp::Element const& element, std::string const& attributeName) {
		auto const* attribute = element.get_attribute(attributeName);

		if (!attribute) 
            return {};

        return attribute->get_value();
    }
}

void PlayListLoader::load(PlayLists& lists) const
{
    lists.clear();
    
    auto const& songs = m_songs.getLoadedSongs();

	for (auto const& node: m_nodeset) {
		auto const& element = dynamic_cast<xmlpp::Element&>(*node);
		auto const name = getAttributeValue(element, "name");

		if (name.empty()) 
            throw PlayListLoadException("Attribute 'name' not found");

        auto playlist = std::make_shared<PlayList>();

        playlist->setName(name);
        
		auto const songNodes = element.find("song");
    	for (auto const& node: songNodes) {
    		auto const& element = dynamic_cast<xmlpp::Element&>(*node);
			auto const title = getAttributeValue(element, "title");
			auto const artist = getAttributeValue(element, "artist");
			auto const song = find(songs, title, artist);

            if(song)
                playlist->addSong(song);
        }
		
        lists.emplace_back(playlist);
	}
}
