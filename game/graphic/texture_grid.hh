#pragma once

#include "graphic/glutil.hh"

#include <vector>
#include <stdexcept>
#include <cstddef>

enum class StretchMode {
    Fixed,    
    Stretch,  
    Tile      
};

struct GridCell {
    float u, v;         
    float uSize, vSize;
    StretchMode modeX;
    StretchMode modeY;
};

class Texture;
class Game;

class TextureGrid {
public:
    TextureGrid();

    void setGrid(const std::vector<float>& cols, const std::vector<float>& rows);
    void setStretchMode(size_t col, size_t row, StretchMode modeX, StretchMode modeY);

    struct RenderCell {
        float u0, v0, u1, v1;
        float dstX, dstY, dstW, dstH;
        StretchMode modeX, modeY;
    };

    std::vector<RenderCell> layout(float targetWidth, float targetHeight) const;

    size_t getColumnCount() const;
    size_t getRowCount() const;

    void draw(Texture& texture, Game& game, float x, float y, float w, float h, float w1, float h1, glmath::vec4 const& color = { 1.f, 1.f, 1.f, 1.f });

private:
    void rebuildCells();

    bool isStretchCol(size_t i) const;
    bool isStretchRow(size_t j) const;

    std::vector<float> columnWidths; 
    std::vector<float> rowHeights;   
    std::vector<GridCell> grid;
};
