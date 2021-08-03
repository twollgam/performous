#pragma once

#include "backgrounds.hh"
#include "screen.hh"
#include "menu.hh"
#include "song.hh"
#include "animvalue.hh"
#include "playlist.hh"
#include "controllers.hh"
#include "songs.hh"
#include "texture.hh"
#include "webcam.hh"
#include "theme.hh"
#include "configuration.hh"
#include <mutex>
#include <vector>

class Audio;
class Database;
class Song;
class Texture;
class ThemePlaylistScreen;
class Backgrounds;
class ThemeInstrumentMenu;

class ScreenPlaylist : public Screen
{
public:
	typedef std::vector< std::shared_ptr<Song> > SongList;
	
	ScreenPlaylist(std::string const& name, Audio& audio, Songs& songs, Backgrounds& bgs);
	
	void manageEvent(input::NavEvent const& event) override;
	void manageEvent(SDL_Event) override;
	void prepare() override;
	void draw() override;
	void enter() override;
	void exit() override;
	void reloadGL() override;
	void triggerSongListUpdate();

	void setPreviousScreen(std::string const&);

private:
	void createSongListMenu();
	void createEscMenu();
	void createSongMenu(int songNumber);
	void drawMenu();
	void createMenuFromPlaylist();
	Texture* loadTextureFromMap(fs::path path);
	SvgTxtTheme& getTextObject(std::string const& txt);
	void draw_menu_options();
	Texture& getCover(Song const& song);

private:
	Menu overlay_menu;
	Menu songlist_menu;
	AnimValue m_selAnim;
	AnimValue m_submenuAnim;
	Audio& m_audio;
	Songs& m_songs;
	Backgrounds& m_backgrounds;
	std::map<fs::path, std::unique_ptr<Texture>> m_covers;
	std::unique_ptr<ThemeInstrumentMenu> m_menuTheme;
	std::unique_ptr<ThemePlaylistScreen> theme;
	std::unique_ptr<Texture> m_background;
	AnimValue m_nextTimer;
	bool keyPressed;
	bool needsUpdate = false;
	mutable std::mutex m_mutex;
	std::unique_ptr<Texture> m_singCover;
	std::unique_ptr<Texture> m_instrumentCover;
	std::unique_ptr<Texture> m_bandCover;
	std::unique_ptr<Texture> m_danceCover;
	std::unique_ptr<Webcam> m_cam;
	std::string m_previousScreen;
};


