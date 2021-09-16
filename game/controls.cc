#include "controls.hh"

#include "configuration.hh"

Control::Control(Control* parent)
: m_parent(parent) {
}

Control* Control::getParent() {
    return m_parent;
}

Control const* Control::getParent() const {
    return m_parent;
}

void Control::setParent(Control* parent) {
    m_parent = parent;
}

void Control::setGeometry(float x, float y, float width, float height) {
    m_x = x;
    m_y = y;
    m_width = width;
    m_height = height;
}
float Control::getX() const {
    return m_x;
}
float Control::getY() const {
    return m_y;
}
float Control::getWidth() const {
    return m_width;
}
float Control::getHeight() const {
    return m_height;
}
void Control::setName(std::string const& name) {
    m_name = name;
}
std::string Control::getName() const {
    return m_name;
}

GraphicContext::GraphicContext() {
    addFont("default", "mainmenu_option.svg"); 
}

void GraphicContext::addFont(std::string const& font, fs::path const& file) {
    m_fonts.emplace(font, std::make_pair(findFile(file), config["graphic/text_lod"].f())); 
}

void GraphicContext::draw(Text& text, float x, float y) {
    text.bind(*this);
    text.draw(x, y);
}

void GraphicContext::draw(Text& text, float x, float y, float width, float height) {
    text.bind(*this);
    text.draw(x, y, width, height);
}

void GraphicContext::drawCentered(Text& text, float x, float y, float width, float height) {
    text.bind(*this);
    text.drawCentered(x, y, width, height);
}

std::shared_ptr<SvgTxtTheme> GraphicContext::makeSvgText(std::string const& text) {
    const auto font = m_fonts.begin()->second;
    
    return std::make_shared<SvgTxtTheme>(font.first, font.second);
}
    

Text::Text(std::string const& text)
: m_text(text)
{
}

void Text::setText(std::string const& text) {
    m_text = text;
    
    if(m_gc && !m_svg)
        m_svg = m_gc->makeSvgText(text);
}

std::string Text::getText() const {
    return m_text;
}

void Text::bind(GraphicContext& gc) {
    m_gc = &gc;
    
    m_svg = gc.makeSvgText(m_text);
}

void Text::draw(float x, float y) {
    m_svg->dimensions.left(x).top(y);
    m_svg->draw(m_text);
}

void Text::draw(float x, float y, float width, float height) {
    m_svg->dimensions.left(x).center(y + height * 0.5);
    m_svg->setAlign(SvgTxtTheme::Align::LEFT);
    m_svg->draw(m_text);
}

void Text::drawCentered(float x, float y, float width, float height) {
    m_svg->dimensions.middle(x + width * 0.5).center(y + height * 0.5);
    m_svg->setAlign(SvgTxtTheme::Align::CENTER);
    m_svg->draw(m_text);
}


Label::Label(std::string const& text, Control* parent)
: Control(parent), m_text(text) {
}

Label::Label(Control* parent, std::string const& text)
: Control(parent), m_text(text) {
}

void Label::setText(std::string const& text) {
    m_text.setText(text);
}

std::string Label::getText() const {
    return m_text.getText();
}
    
void Label::draw(GraphicContext& gc) {
    gc.draw(m_text, getX(), getY(), getWidth(), getHeight());
}


Select::Select(std::vector<std::string> const& items, Control* parent)
: Control(parent), m_text(""), m_background(findFile("mainmenu_comment_bg.svg")), m_items(items) {
}

Select::Select(Control* parent, std::vector<std::string> const& items)
: Control(parent), m_text(""), m_background(findFile("mainmenu_comment_bg.svg")), m_items(items) {
}

void Select::setItems(std::vector<std::string> const& items) {
    m_items = items;
    
    select(0);
}

std::vector<std::string> Select::getItems() const {
    return m_items;
}

size_t Select::countItems() const {
    return m_items.size();
}

std::string Select::getSelectedText() const {
    return m_items.at(m_selected);
}

size_t Select::getSelectedIndex() const {
    return m_selected;
}

void Select::select(size_t index) {
    m_selected = index;
    
    m_text.setText(getSelectedText());
}

void Select::onKey(Key key) {
    if(countItems() == 0)
        return;
    
    switch(key) {
        case Key::Up:
            select((m_selected + 1) % countItems());
            break;
        case Key::Down:
            select((countItems() + m_selected - 1) % countItems());
            break;
        default:;
    }
}

void Select::draw(GraphicContext& gc) {
    const auto color = ColorTrans(hasFocus() ? Color(1.f, 1.f, 1.f) : Color(0.6f, 0.6f, 0.6f));

    m_background.dimensions.left(getX()).top(getY()).stretch(getWidth(), getHeight());
    m_background.draw();
    
    gc.drawCentered(m_text, getX(), getY(), getWidth(), getHeight());
}


TextBox::TextBox(std::string const& text, Control* parent)
: Control(parent), m_text(text),  m_background(findFile("mainmenu_comment_bg.svg")) {
}

TextBox::TextBox(Control* parent, std::string const& text)
: Control(parent), m_text(text),  m_background(findFile("mainmenu_comment_bg.svg")) {
}

TextBox& TextBox::setText(std::string const& text) {
    m_text.setText(text);
    
    m_cursorPosition = text.size();
    
    return *this;
}

std::string TextBox::getText() const {
    auto text = m_text.getText();
    
    if(m_cursorPosition < text.size() && text[m_cursorPosition] == '|')
        text.erase(m_cursorPosition, 1);

    return text;
}

TextBox& TextBox::setMaxLength(size_t length) {
    m_maxLength = length;
    
    return *this;
}

size_t TextBox::getMaxLength() const {
    return m_maxLength;
}

void TextBox::onKey(Key key) {
    auto text = m_text.getText();
    
    if(m_cursorPosition < text.size() && text[m_cursorPosition] == '|')
        text.erase(m_cursorPosition, 1);

    switch(key) {
        case Key::Up:
        case Key::Left:
            if(m_cursorPosition > 0)
                --m_cursorPosition;
            break;
        case Key::Down:
        case Key::Right:
            if(m_cursorPosition < text.size())
                ++m_cursorPosition;
            break;
        case Key::BackSpace:
            if(m_cursorPosition > 0)
                text.erase(--m_cursorPosition, 1);
            break;
        case Key::Delete:
            if(m_cursorPosition < text.size())
                text.erase(m_cursorPosition, 1);
            break;
        default:
            if(text.length() < m_maxLength) {
                if(key >= Key::Number0 && key <= Key::Number9)
                    text.insert(m_cursorPosition++, 1, '0' + static_cast<char>(key) - static_cast<char>(Key::Number0));
                else if(key >= Key::a && key <= Key::z)
                    text.insert(m_cursorPosition++, 1, 'a' + static_cast<char>(key) - static_cast<char>(Key::a));
                else if(key >= Key::A && key <= Key::Z)
                    text.insert(m_cursorPosition++, 1, 'A' + static_cast<char>(key) - static_cast<char>(Key::A));
                else if(key == Key::Space)
                    text.insert(m_cursorPosition++, 1, ' ');
            }
    }
    
    if(m_cursorPosition < text.size() && text[m_cursorPosition] != '|')
        text.insert(m_cursorPosition, 1, '|');
    else if(m_cursorPosition == text.size())
        text.append(1, '|');
    
    m_text.setText(text);
}

void TextBox::draw(GraphicContext& gc) {
    const auto color = ColorTrans(hasFocus() ? Color(1.f, 1.f, 1.f) : Color(0.6f, 0.6f, 0.6f));

    m_background.dimensions.left(getX()).top(getY()).stretch(getWidth(), getHeight());
    m_background.draw();
    
    if(hasFocus()) {
        auto text = m_text.getText();
        
        if(m_cursorPosition < text.size() && text[m_cursorPosition] != '|')
            text.insert(m_cursorPosition, 1, '|');
        else if(m_cursorPosition == text.size())
            text.append(1, '|');
        
        m_text.setText(text);
    }
    else {
        auto text = m_text.getText();
        
        if(m_cursorPosition < text.size() && text[m_cursorPosition] == '|')
            text.erase(m_cursorPosition, 1);
        
        m_text.setText(text);
    }
    
    gc.draw(m_text, getX(), getY(), getWidth(), getHeight());
    
    if(hasFocus()) {
        // draw cursor
    }
}

    
void UserControl::addControl(Control* child) {
    m_children.insert(child);
    
    child->setParent(this);
}

void UserControl::addControl(Control& child) {
    m_children.insert(&child);
    
    child.setParent(this);
}

void UserControl::removeControl(Control* child) {
    m_children.erase(child);
    
    child->setParent(nullptr);
}

void UserControl::removeControl(Control& child) {
    m_children.erase(&child);
    
    child.setParent(nullptr);
}

std::set<Control*> UserControl::getChildren() const {
    return m_children;
}

void UserControl::draw(GraphicContext& gc) {
    for(auto* child : m_children)
        child->draw(gc);
}

void Form::focusNext() {
    if(getChildren().empty())
        return;
    
    if(!m_focus)
        m_focus = *getChildren().begin();
    else {
        auto children = getChildren();
        auto it = children.find(m_focus);
        
        m_focus->setFocus(false);
        
        if(it == children.end() || ++it == children.end())
            m_focus = *getChildren().begin();
        else
            m_focus = *it;
    }
     
    if(m_focus->canFocused())
        m_focus->setFocus(true);
    else
        focusNext();
}

void Form::focusPrevious() {
    if(getChildren().empty())
        return;
    
    if(!m_focus)
        m_focus = *getChildren().rbegin();
    else {
        auto children = getChildren();
        auto it = children.find(m_focus);
        
        m_focus->setFocus(false);
        
        if(it == children.begin() || --it == children.begin())
            m_focus = *getChildren().rbegin();
        else
            m_focus = *it;
    }
        
    m_focus->setFocus(true);    
     
    if(m_focus->canFocused())
        m_focus->setFocus(true);
    else
        focusPrevious();
}

void Form::onKey(Key key) {
    switch(key) {
        case Key::BackTab:
            focusPrevious();
            break;
        case Key::Tab:
            focusNext();
            break;
        default:
            if(m_focus)
                m_focus->onKey(key);
    }
}

