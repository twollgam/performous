#pragma once

#include "backgrounds.hh"
#include "screen.hh"
#include "menu.hh"
#include "song.hh"
#include "animvalue.hh"
#include "playlist.hh"
#include "songs.hh"
#include "configuration.hh"
#include "theme.hh"

#include <memory>
#include <mutex>
#include <vector>

class ScreenAction {
public:
	ScreenAction(Screen& screen) : m_screen(screen) {}
	virtual ~ScreenAction() = default;

    virtual void enter() {}
    virtual void exit() {}
	virtual void draw() {}
    virtual void manageEvent(input::NavEvent const&) {}
	virtual void manageEvent(SDL_Event) {}

private:
	Screen& m_screen;
};

using ScreenActionPtr = std::shared_ptr<ScreenAction>;

class ScreenPlaylistManager : public Screen
{
public:
	ScreenPlaylistManager(std::string const& name, Audio& audio, Songs& songs, Backgrounds& bgs, PlayLists& playlists);

    void manageEvent(input::NavEvent const& event) override;
	void manageEvent(SDL_Event) override;
	void draw() override;
	void enter() override;
	void exit() override;

	void reloadGL() override;
	void prepare() override;

	void triggerSongListUpdate();

	void setAction(const ScreenActionPtr& = {});
    void onActionExit();

private:
	void createSongListMenu(size_t firstDisplayedItem = 0);
	void createPlayListMenu(size_t songNumber);
	void createEscMenu();
	void drawMenu();
	void drawMenuOptions();
	void drawCovers();

	Texture& getCover(Song const& song);
	Texture* loadTextureFromMap(fs::path path);
	SvgTxtTheme& getTextObject(std::string const& txt);
/*
	void createMenuFromPlaylist();
*/
private:
	Audio& m_audio;
	Songs& m_songs;
	Backgrounds& m_backgrounds;

	std::unique_ptr<Texture> m_background;
	Menu m_menu;
	Menu m_overlayMenu;
	std::map<fs::path, std::unique_ptr<Texture>> m_covers;
	std::unique_ptr<Texture> m_singCover;
	std::unique_ptr<Texture> m_instrumentCover;
	std::unique_ptr<Texture> m_bandCover;
	std::unique_ptr<Texture> m_danceCover;
	std::unique_ptr<ThemePlaylistScreen> m_theme;
	std::unique_ptr<ThemeInstrumentMenu> m_menuTheme;
	AnimValue m_selAnim;
	AnimValue m_submenuAnim;
	AnimValue m_nextTimer;
	bool m_keyPressed = false;
	bool m_needsUpdate = false;
	PlayLists& m_playlists;
	PlayListPtr m_selectedList;
	ScreenActionPtr m_action;
	size_t m_firstDisplayedOption = 0;
/*
	bool m_first;
	mutable std::mutex m_mutex;
*/
};
