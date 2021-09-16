#include "screen_songfilter.hh"
    
#include "i18n.hh"
#include "game.hh"
#include "songs.hh"
#include "songfilter.hh"
#include "util.hh"
    
#include <algorithm>

namespace {
    const std::set<std::string> genreTerms{ 
        "Alternative", "Country", "Rock", "Pop", "Metal", "Punk", "Folk", "Grunge",
        "Reggae", "Dance", "House", "Soul", "Musical", "Ballad", "Soundtrack", "Blues", 
        "New age", "R&B", "Oldie", "Ska",
        "NDW"};
    
    struct SongsSummery {
        std::vector<std::string> languages;
        std::vector<std::string> genres;
    };
    
    std::set<std::string> findMatch(std::string const& word, std::set<std::string> const& terms) {
        auto result = std::set<std::string>();
        
        for(auto const& term : terms) {
            if(containsNoCase(word, term))
                result.emplace(term);
        }
        
        if(result.empty())
            result.emplace("Other");
        
        return result;
    }
    
    SongsSummery collectSummery(Songs const& songs) {
        const auto none = "<" + _("None") + ">";
        auto result = SongsSummery();
        
        result.languages.emplace_back(none);
        result.genres.emplace_back(none);
        
        auto languages = std::set<std::string>();
        auto genres = std::set<std::string>();
        
        for(auto const& song : songs.getSongs()) {
            languages.emplace(song->language);
            std::cout << song->genre << std::endl;
            genres.merge(findMatch(song->genre, genreTerms));
        }
        
        std::copy(languages.begin(), languages.end(), std::back_inserter(result.languages));
        std::copy(genres.begin(), genres.end(), std::back_inserter(result.genres));

        return result;
    }
}

ScreenSongFilter::ScreenSongFilter(Game& game, Songs const& songs)
: Screen("SongFilter"), m_game(game), m_songs(songs), m_background(findFile("intro_bg.svg")) {
    initializeControls();
}

void ScreenSongFilter::initializeControls() {
    const auto songSummery = collectSummery(m_songs);
    const auto verticalSpace = 0.05;
    const auto verticalOffset = -0.15;
    const auto horizontalSpace = 0.225;
    const auto horizontalOffset = -0.45;
    const auto lineHeight = 0.04;
    const auto none = "<" + _("None") + ">";
    auto n = 0;
        
    m_control.addControl(m_labelLanguage);
    m_labelLanguage.setText(_("Language") + ":");
    m_labelLanguage.setGeometry(horizontalOffset, verticalOffset + n * verticalSpace, horizontalSpace, lineHeight);
    m_control.addControl(m_selectLanguage0);
    m_selectLanguage0.setItems(songSummery.languages);
    m_selectLanguage0.select(0);
    m_selectLanguage0.setGeometry(horizontalOffset + horizontalSpace, verticalOffset + n * verticalSpace, 0.2, lineHeight);
    m_control.addControl(m_selectLanguage1);
    m_selectLanguage1.setItems(songSummery.languages);
    m_selectLanguage1.select(0);
    m_selectLanguage1.setGeometry(horizontalOffset + horizontalSpace * 2, verticalOffset + n * verticalSpace, 0.2, lineHeight);
    m_control.addControl(m_selectLanguage2);
    m_selectLanguage2.setItems(songSummery.languages);
    m_selectLanguage2.select(0);
    m_selectLanguage2.setGeometry(horizontalOffset + horizontalSpace * 3, verticalOffset + n * verticalSpace, 0.2, lineHeight);

    ++n;
    
    m_control.addControl(m_labelGenre);
    m_labelGenre.setText(_("Genre") + ":");
    m_labelGenre.setGeometry(horizontalOffset, verticalOffset + n * verticalSpace, horizontalSpace, lineHeight);
    m_control.addControl(m_selectGenre0);
    m_selectGenre0.setItems(songSummery.genres);
    m_selectGenre0.select(0);
    m_selectGenre0.setGeometry(horizontalOffset + horizontalSpace, verticalOffset + n * verticalSpace, 0.2, lineHeight);
    m_control.addControl(m_selectGenre1);
    m_selectGenre1.setItems(songSummery.genres);
    m_selectGenre1.select(0);
    m_selectGenre1.setGeometry(horizontalOffset + horizontalSpace * 2, verticalOffset + n * verticalSpace, 0.2, lineHeight);
    m_control.addControl(m_selectGenre2);
    m_selectGenre2.setItems(songSummery.genres);
    m_selectGenre2.select(0);
    m_selectGenre2.setGeometry(horizontalOffset + horizontalSpace * 3, verticalOffset + n * verticalSpace, 0.2, lineHeight);

    ++n;
    
    m_control.addControl(m_labelYear);
    m_labelYear.setText(_("Year") + ":");
    m_labelYear.setGeometry(horizontalOffset, verticalOffset + n * verticalSpace, horizontalSpace, lineHeight);
    m_control.addControl(m_selectYear0);
    m_selectYear0.setItems({none, "1960s", "1970s", "1980s", "1990s", "> 2000"});
    m_selectYear0.select(0);
    m_selectYear0.setGeometry(horizontalOffset + horizontalSpace, verticalOffset + n * verticalSpace, 0.2, lineHeight);
    m_control.addControl(m_selectYear1);
    m_selectYear1.setItems({none, "1960s", "1970s", "1980s", "1990s", "> 2000"});
    m_selectYear1.select(0);
    m_selectYear1.setGeometry(horizontalOffset + horizontalSpace * 2, verticalOffset + n * verticalSpace, 0.2, lineHeight);
    m_control.addControl(m_selectYear2);
    m_selectYear2.setItems({none, "1960s", "1970s", "1980s", "1990s", "> 2000"});
    m_selectYear2.select(0);
    m_selectYear2.setGeometry(horizontalOffset + horizontalSpace * 3, verticalOffset + n * verticalSpace, 0.2, lineHeight);

    ++n;
    
    m_control.addControl(m_labelMode);
    m_labelMode.setText(_("Mode") + ":");
    m_labelMode.setGeometry(horizontalOffset, verticalOffset + n * verticalSpace, horizontalSpace, lineHeight);
    m_control.addControl(m_selectMode);
    m_selectMode.setItems({none, "Solo", "Duet", "Dance", "Drums", "Keyboard", "Guitar"});
    m_selectMode.select(0);
    m_selectMode.setGeometry(horizontalOffset + horizontalSpace, verticalOffset + n * verticalSpace, 0.2, lineHeight);

    ++n;
    
    m_control.addControl(m_labelTitle);
    m_labelTitle.setText(_("Title") + ":");
    m_labelTitle.setGeometry(horizontalOffset, verticalOffset + n * verticalSpace, horizontalSpace, lineHeight);
    m_control.addControl(m_textBoxTitle);
    m_textBoxTitle.setGeometry(horizontalOffset + horizontalSpace, verticalOffset + n * verticalSpace, 0.2 + horizontalSpace * 2, lineHeight);
    m_textBoxTitle.setMaxLength(24);

    ++n;
    
    m_control.addControl(m_labelArtist);
    m_labelArtist.setText(_("Artist") + ":");
    m_labelArtist.setGeometry(horizontalOffset, verticalOffset + n * verticalSpace, horizontalSpace, lineHeight);
    m_control.addControl(m_textBoxArtist);
    m_textBoxArtist.setGeometry(horizontalOffset + horizontalSpace, verticalOffset + n * verticalSpace, 0.2 + horizontalSpace * 2, lineHeight);
    m_textBoxArtist.setMaxLength(24);

    ++n;
    ++n;
    ++n;
    ++n;

    m_control.addControl(m_labelResults);
    m_labelResults.setGeometry(horizontalOffset, verticalOffset + n * verticalSpace, horizontalSpace, lineHeight);

    m_control.focusNext();
    
    updateResult();
}

namespace {
    SongFilterPtr makeYearFilter(size_t n) {
        switch(n) {
            case 1:
                return std::make_shared<YearFilter>(1960, 1969);
            case 2:
                return std::make_shared<YearFilter>(1970, 1979);
            case 3:
                return std::make_shared<YearFilter>(1980, 1989);
            case 4:
                return std::make_shared<YearFilter>(1990, 1999);
            case 5:
                return std::make_shared<YearFilter>(2000, 2999);
            default:
                return std::make_shared<YearFilter>(0, 2999);
        }
    }
}

SongFilterPtr ScreenSongFilter::makeFilter() const {
    auto result = std::make_shared<AndFilter>();
    auto languageFilter = std::make_shared<OrFilter>();
    
    if(m_selectLanguage0.getSelectedIndex() != 0)
        languageFilter->add(std::make_shared<LanguageFilter>(m_selectLanguage0.getSelectedText()));
    if(m_selectLanguage1.getSelectedIndex() != 0)
        languageFilter->add(std::make_shared<LanguageFilter>(m_selectLanguage1.getSelectedText()));
    if(m_selectLanguage2.getSelectedIndex() != 0)
        languageFilter->add(std::make_shared<LanguageFilter>(m_selectLanguage2.getSelectedText()));
    
    if(!languageFilter->isEmpty())
        result->add(languageFilter);
    
    auto genreFilter = std::make_shared<OrFilter>();
    
    if(m_selectGenre0.getSelectedIndex() != 0)
        genreFilter->add(std::make_shared<GenreFilter>(m_selectGenre0.getSelectedText(), genreTerms));
    if(m_selectGenre1.getSelectedIndex() != 0)
        genreFilter->add(std::make_shared<GenreFilter>(m_selectGenre1.getSelectedText(), genreTerms));
    if(m_selectGenre2.getSelectedIndex() != 0)
        genreFilter->add(std::make_shared<GenreFilter>(m_selectGenre2.getSelectedText(), genreTerms));
    
    if(!genreFilter->isEmpty())
        result->add(genreFilter);
    
    auto yearFilter = std::make_shared<OrFilter>();
    
    if(m_selectYear0.getSelectedIndex() != 0)
        yearFilter->add(makeYearFilter(m_selectYear0.getSelectedIndex()));
    if(m_selectYear1.getSelectedIndex() != 0)
        yearFilter->add(makeYearFilter(m_selectYear1.getSelectedIndex()));
    if(m_selectYear2.getSelectedIndex() != 0)
        yearFilter->add(makeYearFilter(m_selectYear2.getSelectedIndex()));
    
    if(!yearFilter->isEmpty())
        result->add(yearFilter);
    
    if(m_selectMode.getSelectedIndex() != 0)
        result->add(std::make_shared<ModeFilter>(static_cast<ModeFilter::Mode>(m_selectMode.getSelectedIndex() - 1)));

    if(!m_textBoxTitle.getText().empty())
        result->add(std::make_shared<TitleFilter>(m_textBoxTitle.getText()));

    if(!m_textBoxArtist.getText().empty())
        result->add(std::make_shared<ArtistFilter>(m_textBoxArtist.getText()));

    return result;
}

void ScreenSongFilter::updateResult() {
    const auto filter = makeFilter();
    const auto songs = m_songs.getSongs();
    const auto all = songs.size();
    const auto filtered = std::count_if(songs.begin(), songs.end(), [&filter](auto const& song) { return filter->filter(*song);});
    
    m_labelResults.setText(_("Result") + ": " + std::to_string(filtered) + " / " + std::to_string(all));
}

void ScreenSongFilter::manageEvent(input::NavEvent const& event) {
	const auto nav = event.button;
    
	if (nav == input::NAV_CANCEL) 
		m_game.activateScreen("Intro");	
	else switch(nav) {
        case input::NAV_RIGHT:
            m_control.onKey(Control::Key::Right);
            break;
        case input::NAV_LEFT:
            m_control.onKey(Control::Key::Left);
            break;
        case input::NAV_UP:
            m_control.onKey(Control::Key::Up);
            break;
        case input::NAV_DOWN:
            m_control.onKey(Control::Key::Down);
            break;
        default:;
    }
    
    updateResult();
}

void ScreenSongFilter::manageEvent(SDL_Event event) {
    if (event.type != SDL_KEYDOWN)
        return;
    
    const auto shift = (event.key.keysym.mod & KMOD_SHIFT) != 0;
    
	switch(event.key.keysym.scancode) {
        case SDL_SCANCODE_ESCAPE:
            m_control.onKey(Control::Key::Escape);
            break;
        case SDL_SCANCODE_RETURN:
            m_control.onKey(Control::Key::Return);
            break;
        case SDL_SCANCODE_SPACE:
            m_control.onKey(Control::Key::Space);
            break;
        case SDL_SCANCODE_BACKSPACE:
            m_control.onKey(Control::Key::BackSpace);
            break;
        case SDL_SCANCODE_DELETE:
            m_control.onKey(Control::Key::Delete);
            break;
        case SDL_SCANCODE_TAB:
            m_control.onKey(shift ? Control::Key::BackTab : Control::Key::Tab);
            break;
        case SDL_SCANCODE_0:
            m_control.onKey(Control::Key::Number0);
            break;
        case SDL_SCANCODE_1:
            m_control.onKey(Control::Key::Number1);
            break;
        case SDL_SCANCODE_2:
            m_control.onKey(Control::Key::Number2);
            break;
        case SDL_SCANCODE_3:
            m_control.onKey(Control::Key::Number3);
            break;
        case SDL_SCANCODE_4:
            m_control.onKey(Control::Key::Number4);
            break;
        case SDL_SCANCODE_5:
            m_control.onKey(Control::Key::Number5);
            break;
        case SDL_SCANCODE_6:
            m_control.onKey(Control::Key::Number6);
            break;
        case SDL_SCANCODE_7:
            m_control.onKey(Control::Key::Number7);
            break;
        case SDL_SCANCODE_8:
            m_control.onKey(Control::Key::Number8);
            break;
        case SDL_SCANCODE_9:
            m_control.onKey(Control::Key::Number9);
            break;
        case SDL_SCANCODE_A:
            m_control.onKey(shift ? Control::Key::A : Control::Key::a);
            break;
        case SDL_SCANCODE_B:
            m_control.onKey(shift ? Control::Key::B : Control::Key::b);
            break;
        case SDL_SCANCODE_C:
            m_control.onKey(shift ? Control::Key::C : Control::Key::c);
            break;
        case SDL_SCANCODE_D:
            m_control.onKey(shift ? Control::Key::D : Control::Key::d);
            break;
        case SDL_SCANCODE_E:
            m_control.onKey(shift ? Control::Key::E : Control::Key::e);
            break;
        case SDL_SCANCODE_F:
            m_control.onKey(shift ? Control::Key::F : Control::Key::f);
            break;
        case SDL_SCANCODE_G:
            m_control.onKey(shift ? Control::Key::G : Control::Key::g);
            break;
        case SDL_SCANCODE_H:
            m_control.onKey(shift ? Control::Key::H : Control::Key::h);
            break;
        case SDL_SCANCODE_I:
            m_control.onKey(shift ? Control::Key::I : Control::Key::i);
            break;
        case SDL_SCANCODE_J:
            m_control.onKey(shift ? Control::Key::J : Control::Key::j);
            break;
        case SDL_SCANCODE_K:
            m_control.onKey(shift ? Control::Key::K : Control::Key::k);
            break;
        case SDL_SCANCODE_L:
            m_control.onKey(shift ? Control::Key::L : Control::Key::l);
            break;
        case SDL_SCANCODE_M:
            m_control.onKey(shift ? Control::Key::M : Control::Key::m);
            break;
        case SDL_SCANCODE_N:
            m_control.onKey(shift ? Control::Key::N : Control::Key::n);
            break;
        case SDL_SCANCODE_O:
            m_control.onKey(shift ? Control::Key::O : Control::Key::o);
            break;
        case SDL_SCANCODE_P:
            m_control.onKey(shift ? Control::Key::P : Control::Key::p);
            break;
        case SDL_SCANCODE_Q:
            m_control.onKey(shift ? Control::Key::Q : Control::Key::q);
            break;
        case SDL_SCANCODE_R:
            m_control.onKey(shift ? Control::Key::R : Control::Key::r);
            break;
        case SDL_SCANCODE_S:
            m_control.onKey(shift ? Control::Key::S : Control::Key::s);
            break;
        case SDL_SCANCODE_T:
            m_control.onKey(shift ? Control::Key::T : Control::Key::t);
            break;
        case SDL_SCANCODE_U:
            m_control.onKey(shift ? Control::Key::U : Control::Key::u);
            break;
        case SDL_SCANCODE_V:
            m_control.onKey(shift ? Control::Key::V : Control::Key::v);
            break;
        case SDL_SCANCODE_W:
            m_control.onKey(shift ? Control::Key::W : Control::Key::w);
            break;
        case SDL_SCANCODE_X:
            m_control.onKey(shift ? Control::Key::X : Control::Key::x);
            break;
        case SDL_SCANCODE_Y:
            m_control.onKey(shift ? Control::Key::Y : Control::Key::y);
            break;
        case SDL_SCANCODE_Z:
            m_control.onKey(shift ? Control::Key::Z : Control::Key::z);
            break;
        default:;
    }
    
    updateResult();
}

void ScreenSongFilter::draw() {
    m_background.draw();
    m_control.draw(m_gc); 
}

void ScreenSongFilter::enter() {
}

void ScreenSongFilter::exit() {
}

