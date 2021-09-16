#pragma once

#include "opengl_text.hh"

#include <string>
#include <set>
#include <vector>

class Text;

class GraphicContext {
public:
    GraphicContext();
    
    void addFont(std::string const& font, fs::path const& file);
    
    void draw(Text& text, float x, float y);
    void draw(Text& text, float x, float y, float width, float height);
    void drawCentered(Text& text, float x, float y, float width, float height);
    
    std::shared_ptr<SvgTxtTheme> makeSvgText(std::string const& text);
    
private:
    std::map<std::string, std::pair<fs::path, double>> m_fonts;
};

class Text {
public:
    Text(std::string const& text = {});
    
    void setText(std::string const& text);
    std::string getText() const;
    void bind(GraphicContext& gc);
    void draw(float x, float y);
    void draw(float x, float y, float width, float height);
    void drawCentered(float x, float y, float width, float height);
    
private:
    GraphicContext* m_gc = nullptr;
    std::string m_text;
    std::shared_ptr<SvgTxtTheme> m_svg;
};

class Control {
  public:
    Control() = default;
    Control(Control* parent);
    virtual ~Control() = default;
    
    Control* getParent();
    Control const* getParent() const;
    void setParent(Control*);
    
    void setGeometry(float x, float y, float width, float height);
    float getX() const;
    float getY() const;
    float getWidth() const;
    float getHeight() const;
    
    void setName(std::string const& name);
    std::string getName() const;

    virtual bool canFocused() const { return true; }
    virtual void setFocus(bool focused) { m_focused = focused; }
    bool hasFocus() const { return m_focused; }
    
    enum class Key { Up, Down, Left, Right, PageUp, PageDown, Space, Tab, BackTab, Escape, Return, Delete, BackSpace,
        Number0, Number1, Number2, Number3, Number4, Number5, Number6, Number7, Number8, Number9, 
        a, b, c, d, e, f, g, h, i, j, k, l, m, n, o, p, q, r, s, t, u, v, w, x, y, z,
        A, B, C, D, E, F, G, H, I, J, K, L, M, N, O, P, Q, R, S, T, U, V, W, X, Y, Z
    };
    
    virtual void onKey(Key) {}
    
    virtual void draw(GraphicContext&) = 0;
  
  protected:
    Control* m_parent = nullptr;

  private:
    std::string m_name;
    float m_x;
    float m_y;
    float m_width;
    float m_height;
    bool m_focused = false;
};

class Label : public Control {
  public:
    Label(std::string const& text = {}, Control* parent = nullptr);
    Label(Control* parent, std::string const& text = {});

    void setText(std::string const& text);
    std::string getText() const;

    bool canFocused() const override { return false; }
    void draw(GraphicContext&) override;

    operator Text&() { return *this; }
        
  private:
    Text m_text;
};


class Select : public Control {
  public:
    Select(std::vector<std::string> const& items = {}, Control* parent = nullptr);
    Select(Control* parent, std::vector<std::string> const& items = {});

    void setItems(std::vector<std::string> const&);
    std::vector<std::string> getItems() const;
    size_t countItems() const;
    
    std::string getSelectedText() const;
    size_t getSelectedIndex() const;

    void select(size_t);

    void onKey(Key) override;
    
    void draw(GraphicContext&) override;
        
  private:
    Text m_text;
    Texture m_background;
    std::vector<std::string> m_items;
    size_t m_selected = -1;
};


class TextBox : public Control {
  public:
    TextBox(std::string const& text = {}, Control* parent = nullptr);
    TextBox(Control* parent, std::string const& text = {});

    TextBox& setText(std::string const&);
    std::string getText() const;
    TextBox& setMaxLength(size_t);
    size_t getMaxLength() const;
    
    void onKey(Key) override;
    
    void draw(GraphicContext&) override;
        
  private:
    Text m_text;
    Texture m_background;
    Texture m_cursor;
    size_t m_cursorPosition = 0;
    size_t m_maxLength = size_t(-1);
};


class UserControl : public Control {
  public:

    void addControl(Control* child);
    void addControl(Control& child);
    void removeControl(Control* child);
    void removeControl(Control& child);
    std::set<Control*> getChildren() const;
    
    void draw(GraphicContext&) override;
        
  private:
      std::set<Control*> m_children;
};

class Form : public UserControl {
  public:
      void focusNext();
      void focusPrevious();
      
      void onKey(Key) override;

  private:
      Control* m_focus = nullptr;
};

