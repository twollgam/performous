#pragma once

#include "screen.hh"
#include "controls.hh"
#include "isongfilter.hh"

class Game;
class Songs;

class ScreenSongFilter : public Screen {
  public:
	ScreenSongFilter(Game&, Songs const&);
	~ScreenSongFilter() override = default; 
    
	void manageEvent(input::NavEvent const& event) override;
	void manageEvent(SDL_Event) override;
	void prepare() override {}
	void draw() override;
	void enter() override;
	void exit() override;
	void reloadGL() { exit(); enter(); }

  private:
    void initializeControls();
    void updateResult();
    SongFilterPtr makeFilter() const;

  private:
	Game& m_game;
    Songs const& m_songs;
    Texture m_background;
    GraphicContext m_gc;
    Form m_control;
    Label m_labelLanguage;
    Select m_selectLanguage0;
    Select m_selectLanguage1;
    Select m_selectLanguage2;
    Label m_labelGenre;
    Select m_selectGenre0;
    Select m_selectGenre1;
    Select m_selectGenre2;
    Label m_labelYear;
    Select m_selectYear0;
    Select m_selectYear1;
    Select m_selectYear2;
    Label m_labelMode;
    Select m_selectMode;
    Label m_labelTitle;
    TextBox m_textBoxTitle;
    Label m_labelArtist;
    TextBox m_textBoxArtist;
    Label m_labelResults;
};

 
