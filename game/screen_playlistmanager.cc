#include "screen_playlistmanager.hh"

#include "audio.hh"
#include "i18n.hh"
#include "menu.hh"
#include "playlist.hh"
#include "screen_playlist.hh"
#include "textinput.hh"
#include "theme.hh"
#include "util.hh"

#include <iostream>
#include <memory>
#include <sstream>


class RenameAction : public ScreenAction {
public:
	RenameAction(ScreenPlaylistManager&, std::string const& text, std::string const& label = "Text:");

	void setOnExit(const std::function<void(const std::string&)>&); 

    void enter() override;
    void exit() override;
	void draw() override;
    void manageEvent(input::NavEvent const& event) override;
	void manageEvent(SDL_Event) override;

private:
	void initialize();

private:
	ScreenPlaylistManager& m_screen;
	std::string m_origin;
	std::string m_label;
	std::function<void(const std::string&)> m_onExit;
	TextInput m_text;
	SvgTxtTheme m_textTheme;
	Texture m_textBackground;
};

namespace{
	const auto MaxDisplayedOptions = 6U;
}

ScreenPlaylistManager::ScreenPlaylistManager(std::string const& name, Audio& audio, Songs& songs, Backgrounds& bgs, PlayLists& playlists)
: Screen(name), m_audio(audio), m_songs(songs), m_backgrounds(bgs), m_playlists(playlists)
{
}

void ScreenPlaylistManager::enter() {
	std::cout << "enter playlist manager" << std::endl;
	
	// temporary, must moved to matching place after song loading 
	if(m_playlists.empty())
	{
	    auto const& songs = m_songs.getLoadedSongs();
	
		if(!songs.empty()) {
			for(auto j = 0; j < 12; ++j)
			{
				m_playlists.emplace_back(std::make_shared<PlayList>());
				m_playlists[j]->setName(std::to_string(j));

				const auto n = rand() % 12;
				for(auto i = 0; i < n; ++i)
					m_playlists[j]->addSong(songs[j * 8 + i]);
			}
		}
	}

	m_audio.togglePause();
	m_keyPressed = false;
	auto timervalue = config["game/playlist_screen_timeout"].i();
	if(timervalue < 0.0) {
		timervalue = 0.0;
	}
	m_nextTimer.setValue(timervalue);

	m_overlayMenu.close();
	Game::getSingleton().loading(_("Loading song timestamps..."), 0.2);
	
	if(!m_selectedList && !m_playlists.empty())
		m_selectedList = m_playlists[0];
	
	createSongListMenu();
	drawCovers();
	m_menu.open();
	reloadGL();
}

void ScreenPlaylistManager::exit() {
	m_overlayMenu.clear();
	m_menu.clear();

	m_menuTheme.reset();
	m_theme.reset();

	m_audio.togglePause();
	m_background.reset();
}

void ScreenPlaylistManager::manageEvent(input::NavEvent const& event) {
	if(m_action)
		m_action->manageEvent(event);
	else
	{	
		if (m_keyPressed == false)
			m_keyPressed = true;

		const auto nav = event.button;
		
		if(m_overlayMenu.isOpen()) {
			if (nav == input::NAV_CANCEL) {
				m_overlayMenu.close();
			} else {
				if (nav == input::NAV_PAUSE) {
					m_audio.togglePause();
				} else if (nav == input::NAV_START) {
					m_overlayMenu.action();
				} else if (event.menu == input::NAVMENU_A_PREV) {
					m_overlayMenu.move(-1);
				} else if (event.menu == input::NAVMENU_A_NEXT) {
					m_overlayMenu.move(1);
				} else if (event.menu == input::NAVMENU_B_PREV) {
					m_overlayMenu.move(-1);
				} else if (event.menu == input::NAVMENU_B_NEXT) {
					m_overlayMenu.move(1);
				}
			}
		}
		else {
			if (nav == input::NAV_CANCEL) {
				createEscMenu();
				m_overlayMenu.open();
			} else {
				if (nav == input::NAV_PAUSE)
					m_audio.togglePause();
				else if (nav == input::NAV_START)
					m_menu.action();

				auto moveUp = false;
				auto moveDown = false;

				switch(event.menu) {
					case input::NAVMENU_A_PREV:
					case input::NAVMENU_B_PREV:
						moveUp = true;
						break;
					case input::NAVMENU_A_NEXT:
					case input::NAVMENU_B_NEXT:
						moveDown = true;
						break;
				}

				if(moveUp) {
					m_menu.move(-1);
					if(m_firstDisplayedOption > m_menu.curIndex())
						m_firstDisplayedOption = m_menu.curIndex();
				}
				else if(moveDown) {
					m_menu.move(1);
					std::cout << "first: " << m_firstDisplayedOption << std::endl;
					std::cout << "cur: " << m_menu.curIndex() << std::endl;
					if(m_firstDisplayedOption + MaxDisplayedOptions <= m_menu.curIndex())
						m_firstDisplayedOption = m_menu.curIndex() - MaxDisplayedOptions + 1;
					std::cout << "first: " << m_firstDisplayedOption << std::endl;
				}

				const auto index = m_menu.curIndex();

				if(index < m_playlists.size())
					m_selectedList = m_playlists[index];
				else
					m_selectedList.reset();

				drawCovers();
			}
		}
	}
}

void ScreenPlaylistManager::manageEvent(SDL_Event event) {
	if(m_action)
		m_action->manageEvent(event);
}

void ScreenPlaylistManager::draw() {
	if (!m_background || m_background->empty()) 
		m_background = std::make_unique<Texture>(m_backgrounds.getRandom());
	m_background->draw();

	drawMenuOptions();
	//menu on top of everything
	if (m_overlayMenu.isOpen()) {
		drawMenu();
	}
	if(m_needsUpdate) {
//		std::lock_guard<std::mutex> l(m_mutex);
		createSongListMenu();
		m_needsUpdate = false;
	}

	drawCovers();

	if(m_action)
		m_action->draw();
}

void ScreenPlaylistManager::drawCovers() {

	if(m_selectedList) {
		const auto maxDisplayedCovers = 16;
		auto const& songlist = m_selectedList->getList();

		for (auto i = 0U; i < songlist.size(); ++i) {
			if(i < maxDisplayedCovers) {
				auto& cover = getCover(*songlist[i]);
				auto position =  float(i) / float(9);
				using namespace glmath;
				Transform trans(
				translate(vec3(-0.4f + 0.9f * position, 0.045f, 0.0f)) //vec3(horizontal-offset-from-center, vertical offset from screen_bottom)
				* rotate(-0.0f, vec3(0.0f, 1.0f, 0.0f))
				);
				cover.dimensions.middle().screenBottom(-0.06).fitInside(0.08, 0.08);
				cover.draw();
			}
		}
	}
}

void ScreenPlaylistManager::prepare() {
}

void ScreenPlaylistManager::reloadGL() {
	m_theme = std::make_unique<ThemePlaylistScreen>();
	m_menuTheme = std::make_unique<ThemeInstrumentMenu>();
	m_singCover = std::make_unique<Texture>(findFile("no_cover.svg"));
	m_instrumentCover = std::make_unique<Texture>(findFile("instrument_cover.svg"));
	m_bandCover = std::make_unique<Texture>(findFile("band_cover.svg"));
	m_danceCover = std::make_unique<Texture>(findFile("dance_cover.svg"));
}

Texture* ScreenPlaylistManager::loadTextureFromMap(fs::path path) {
	if(m_covers.find(path) == m_covers.end()) {
		auto kv = std::make_pair(path, std::make_unique<Texture>(path));
		m_covers.insert(std::move(kv));
	}
	try {
		return m_covers.at(path).get();
	} 
	catch (std::exception const&) {		
	}

	return nullptr;
}

Texture& ScreenPlaylistManager::getCover(Song const& song) {
	Texture* cover = nullptr;
	// Fetch cover image from cache or try loading it
	if (!song.cover.empty()) 
		cover = loadTextureFromMap(song.cover);
	// Fallback to background image as cover if needed
	if (!cover && !song.background.empty()) 
		cover = loadTextureFromMap(song.background);
	// Use empty cover
	if (!cover) {
		if(song.hasDance()) {
			cover = m_danceCover.get();
		} else if(song.hasDrums()) {
			cover = m_bandCover.get();
		} else {
			size_t tracks = song.instrumentTracks.size();
			if (tracks == 0) 
				cover = m_singCover.get();
			else 
				cover = m_instrumentCover.get();
		}
	}

	return *cover;
}

void ScreenPlaylistManager::drawMenu() {
	if (m_overlayMenu.empty()) 
		return;
	// Some helper vars
	auto& th = *m_menuTheme;
	const auto cur = &m_overlayMenu.current();
	double w = m_overlayMenu.dimensions.w();
	const float txth = th.option_selected.h();
	const float step = txth * 0.85f;
	const float h = m_overlayMenu.getOptions().size() * step + step;
	float y = -h * .5f + step;
	float x = -w * .5f + step;
	// Background
	th.bg.dimensions.middle(0).center(0).stretch(w, h);
	th.bg.draw();
	// Loop through menu items
	w = 0;
	for (auto it = m_overlayMenu.begin(); it != m_overlayMenu.end(); ++it) {
		// Pick the font object
		auto* txt = &th.option_selected;
		if (cur != &*it)
			txt = &(th.getCachedOption(it->getName()));
		// Set dimensions and draw
		txt->dimensions.middle(x).center(y);
		txt->draw(it->getName());
		w = std::max(w, txt->w() + 2 * step); // Calculate the widest entry
		y += step;
	}
	if (cur->getComment() != "") {
		th.comment.dimensions.middle(0).screenBottom(-0.12);
		th.comment.draw(cur->getComment());
	}
	m_overlayMenu.dimensions.stretch(w, h);
}

void ScreenPlaylistManager::drawMenuOptions() {
	double wcounter = 0;
	auto const x = -0.35; // x xcoordinate from screen center, the menu should be aligned left of the center therefore it´s negative.n
	auto const start_y = -0.15;
	auto const sel_margin = 0.04;
	auto const& options = m_menu.getOptions();
	auto const submenuanim = 1.0 - std::min(1.0, std::abs(m_submenuAnim.get() - m_menu.getSubmenuLevel()));
	auto const startItem = m_firstDisplayedOption;

	for (auto i = 0; i < MaxDisplayedOptions && startItem + i < options.size(); ++i) {
		auto const& option = options[startItem + i];
		auto const color = ColorTrans(Color::alpha(submenuanim));

		if (startItem + i == m_menu.curIndex()) {
			// Animate selection higlight moving
			auto selanim = m_selAnim.get() - startItem;
			if (selanim < 0) 
				selanim = 0;
			// Draw the text, dim if option not available
			{
				const auto color = ColorTrans(Color::alpha(option.isActive() ? 1.0 : 0.5));

				m_theme->option_selected.dimensions.left(x).center(start_y + i*0.049);
				m_theme->option_selected.draw(option.getName());
			}
			wcounter = std::max(wcounter, m_theme->option_selected.w() + 2 * sel_margin); // Calculate the widest entry
			// If this is a config item, show the value below
			if (option.type == MenuOption::CHANGE_VALUE) {
				++i; // Use a slot for the value
				m_theme->option_selected.dimensions.left(x + sel_margin).center(-0.1 + (selanim+1)*0.08);
				m_theme->option_selected.draw("<  " + option.value->getValue() + "  >");
			}

		// Regular option (not selected)
		} else {
			const auto title = option.getName();
			auto& txt = getTextObject(title);
			const auto color = ColorTrans(Color::alpha(option.isActive() ? 1.0 : 0.5));

			txt.dimensions.left(x).center(start_y + i*0.05);
			txt.draw(title);
			wcounter = std::max(wcounter, txt.w() + 2 * sel_margin); // Calculate the widest entry
		}
	}
	m_menu.dimensions.stretch(wcounter, 1);
}

SvgTxtTheme& ScreenPlaylistManager::getTextObject(std::string const& txt) {
	if (m_theme->options.find(txt) != m_theme->options.end()) 
		return *m_theme->options.at(txt);

	auto kv = std::make_pair(txt, std::make_unique<SvgTxtTheme>(findFile("mainmenu_option.svg"), config["graphic/text_lod"].f()));
	
	m_theme->options.insert(std::move(kv));

	return *m_theme->options.at(txt);
}

void ScreenPlaylistManager::createEscMenu() {
	m_overlayMenu.clear();
	m_overlayMenu.add(MenuOption(_("Exit"), _("Return to intro screen")).call([this]() {
		m_overlayMenu.close();
		Game::getSingleton().activateScreen("Intro");
	}));
	m_overlayMenu.add(MenuOption(_("Back"), _("Back to playlist manager")).call([this]() {
		m_overlayMenu.close();
	}));
}

void ScreenPlaylistManager::createSongListMenu(size_t firstDisplayedItem) {
	m_menu.clear();

	auto count = 0;
	for (auto const& list: m_playlists) {
		const auto title = "#" + std::to_string(count) + ":  " + list->getName() + "  (" + std::to_string(list->getList().size()) + " Songs)";
		m_menu.add(MenuOption(_(title),_("Press enter to view list options")).call(
			[this, count]() {
				createPlayListMenu(count);
				m_overlayMenu.open();
				m_selectedList = m_playlists[count];
			}
		));
		++count;
	}
	m_menu.add(MenuOption(_("Create new playlist"), _("Add new empty playlist")).call(
		[this]() {
			m_menu.close();

			auto action = std::make_shared<RenameAction>(*this, "", "Name:");

			action->setOnExit(
				[this](const std::string& name){
					auto const newList = std::make_shared<PlayList>();

					newList->setName(name);
					m_playlists.emplace_back(newList); 

					createSongListMenu();
				}
			);

			setAction(action);
		}
	));
	m_menu.add(MenuOption(_("View more options"),_("View general playlist settings")).call([this]() {
		createEscMenu();
		m_overlayMenu.open();
	}));

	m_menu.select(firstDisplayedItem);
}

void ScreenPlaylistManager::createPlayListMenu(size_t listNumber) {
	const auto size = m_playlists.size();
	auto const isFirstSong = listNumber == 0;
	auto const isLastSong = listNumber == size - 1;

	m_overlayMenu.clear();
	m_overlayMenu.add(MenuOption(_("Change content"), _("Add songs to or remove songs from this playlist")).call(
		[this, listNumber]() {
			auto& game = Game::getSingleton();
			auto playlistScreen = dynamic_cast<ScreenPlaylist*>(game.getScreen("Playlist"));

			if(playlistScreen)
				playlistScreen->setPreviousScreen("PlaylistManager");

			game.setPlayList(m_playlists[listNumber]);			
			game.activateScreen("Playlist");
		}
	));
	m_overlayMenu.add(MenuOption(_("Rename"), _("Rename this playlist")).call(
		[this, listNumber]() {
			m_overlayMenu.close();

			auto action = std::make_shared<RenameAction>(*this, m_selectedList->getName(), "Name:");

			action->setOnExit(
				[this](const std::string& name){ 
					m_selectedList->setName(name);
					createSongListMenu();
				}
			);

			setAction(action);
		}
	));
	m_overlayMenu.add(MenuOption(_("Clone"), _("Clone this playlist")).call(
		[this, listNumber]() {
			m_overlayMenu.close();

			auto action = std::make_shared<RenameAction>(*this, m_selectedList->getName(), "Name:");

			action->setOnExit(
				[this, listNumber](const std::string& name){ 
					auto const clone = std::make_shared<PlayList>(*m_selectedList);
					
					clone->setName(name);
					m_playlists.insert(m_playlists.begin() + listNumber + 1, clone);

					createSongListMenu();
				}
			);

			setAction(action);
		}
	));
	if (!isLastSong) {
		m_overlayMenu.add(MenuOption(_("Merge"), _("Merge the next playlist into this playlist")).call(
			[this, listNumber]() {
				m_overlayMenu.close();

				auto const nextList = m_playlists[listNumber + 1];

				for(auto const& song : nextList->getList())
					m_selectedList->addSong(song);				

				createSongListMenu();
			}
		));
	}
	m_overlayMenu.add(MenuOption(_("Clear"), _("Remove all songs from this playlist")).call(
		[this, listNumber]() {
			m_overlayMenu.close();
			m_selectedList->clear();
			createSongListMenu();
		}
	));
	m_overlayMenu.add(MenuOption(_("Remove"), _("Remove this playlist")).call(
		[this, listNumber]() {
			m_overlayMenu.close();
			m_playlists.erase(m_playlists.begin() + listNumber);
			createSongListMenu();
		}
	));
	if (!isFirstSong) {
		m_overlayMenu.add(MenuOption(_("Move up"), _("Move this playlist up")).call(
			[this, listNumber]() {
				m_overlayMenu.close();
				std::iter_swap(m_playlists.begin() + listNumber, m_playlists.begin() + listNumber - 1);
				createSongListMenu();
			}
		));
	}
	if (!isLastSong) {
		m_overlayMenu.add(MenuOption(_("Move Down"), _("Move this playlist down")).call(
			[this, listNumber]() {
				m_overlayMenu.close();
				std::iter_swap(m_playlists.begin() + listNumber, m_playlists.begin() + listNumber + 1);
				createSongListMenu();
			}
		));
	}
	m_overlayMenu.add(MenuOption(_("Back"), _("Back to playlist manager")).call(
		[this]() {
			m_overlayMenu.close();
		}
	));
}

void ScreenPlaylistManager::triggerSongListUpdate() {
	//std::lock_guard<std::mutex> l (m_mutex);
	m_needsUpdate = true;
}

void ScreenPlaylistManager::setAction(const ScreenActionPtr& action) {
	if(m_action)
		m_action->exit();

	m_action = action;

	if(m_action)
		m_action->enter();
}

void ScreenPlaylistManager::onActionExit() {

}





RenameAction::RenameAction(ScreenPlaylistManager& screen, std::string const& text, std::string const& label)
: ScreenAction(screen), m_screen(screen), m_origin(text), m_label(label),
    m_textTheme(findFile("mainmenu_option.svg"), config["graphic/text_lod"].f()),
	m_textBackground(findFile("mainmenu_comment_bg.svg"))
{
	m_text.text = text;
}

void RenameAction::setOnExit(const std::function<void(const std::string&)>& callback) {
	m_onExit = callback;
} 

void RenameAction::enter() {
	std::cout << "RenameAction::enter" << std::endl;
	initialize();
	draw();
}

void RenameAction::exit() {
	std::cout << "RenameAction::exit" << std::endl;
	std::cout << "text: " << m_text.text << std::endl;

	if(m_onExit)
		m_onExit(m_text.text);

	m_screen.onActionExit();
}

void RenameAction::manageEvent(input::NavEvent const& event) {
	std::cout << "RenameAction::manageEvent" << std::endl;

	const auto nav = event.button;

	if (nav == input::NAV_CANCEL) {
		m_text.text = m_origin;
		m_screen.setAction();
	}
	else if (nav == input::NAV_START) 
		m_screen.setAction();
}

void RenameAction::manageEvent(SDL_Event event) {
	std::cout << "RenameAction::manageEvent" << std::endl;

	//keyPressed = true;
	if (event.type == SDL_TEXTINPUT) {
		std::cout << "input:" << event.text.text << std::endl;
		m_text += event.text.text;
	}
	else if (event.type == SDL_KEYDOWN) {
		const auto key = event.key.keysym.scancode;
		if (key == SDL_SCANCODE_BACKSPACE) {
			std::cout << "backspace" << std::endl;
			m_text.backspace();
		}
	}
}

void RenameAction::initialize() {
	//m_textBackground.dimensions = Dimensions(0.25, 0.25, 0.5, 0.1);
    //m_textTheme.dimensions = Dimensions(0.25, 0.5, 0.5, 0.1);
}

void RenameAction::draw() {
	m_textBackground.dimensions.center().middle().stretch(0.755, 0.05);
    m_textTheme.dimensions.center(0.05).middle().stretch(0.75, 0.1);

	const auto bgcolor = ColorTrans(Color());
	m_textBackground.draw();

	const auto color = ColorTrans(Color(0,0,0));
    m_textTheme.draw(m_label + " " + m_text.text);
}
