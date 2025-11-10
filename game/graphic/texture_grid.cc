#include "texture_grid.hh"

#include "game.hh"
#include "texture.hh"

#include <algorithm>
#include "indexed_vertex_buffer.hh"

TextureGrid::TextureGrid() {
    columnWidths = { 1.0f };
    rowHeights = { 1.0f };
    rebuildCells();
    grid[0].modeX = StretchMode::Stretch;
    grid[0].modeY = StretchMode::Stretch;
}

void TextureGrid::setGrid(const std::vector<float>& cols, const std::vector<float>& rows) {
    columnWidths = cols;
    rowHeights = rows;
    rebuildCells();
}

void TextureGrid::setStretchMode(size_t col, size_t row, StretchMode modeX, StretchMode modeY) {
    if (row >= rowHeights.size() || col >= columnWidths.size())
        throw std::out_of_range("Invalid grid index");
    grid[row * columnWidths.size() + col].modeX = modeX;
    grid[row * columnWidths.size() + col].modeY = modeY;
}

std::vector<TextureGrid::RenderCell> TextureGrid::layout(float targetWidth, float targetHeight) const {
    std::vector<RenderCell> output;
    auto totalFixedWidth = 0.f;
    auto totalFixedHeight = 0.f;
    auto stretchCols = 0, stretchRows = 0;

    for (size_t i = 0; i < columnWidths.size(); ++i)
        (isStretchCol(i) ? stretchCols : totalFixedWidth += columnWidths[i]);
    for (size_t j = 0; j < rowHeights.size(); ++j)
        (isStretchRow(j) ? stretchRows : totalFixedHeight += rowHeights[j]);

    auto stretchWidth = (targetWidth - totalFixedWidth * targetWidth) / std::max(1, stretchCols);
    auto stretchHeight = (targetHeight - totalFixedHeight * targetHeight) / std::max(1, stretchRows);

    auto y = 0.f;
    for (size_t j = 0; j < rowHeights.size(); ++j) {
        float h = isStretchRow(j) ? rowHeights[j] * targetHeight : rowHeights[j];
        float x = 0;
        for (size_t i = 0; i < columnWidths.size(); ++i) {
            float w = isStretchCol(i) ? columnWidths[i] * targetWidth : columnWidths[i];
            const GridCell& cell = grid[j * columnWidths.size() + i];

            output.push_back({
                cell.u,
                cell.v,
                cell.u + cell.uSize,
                cell.v + cell.vSize,
                x, y, w, h,
                cell.modeX, cell.modeY
                });

            x += w;
        }
        y += h;
    }
    return output;
}

size_t TextureGrid::getColumnCount() const {
    return columnWidths.size();
}

size_t TextureGrid::getRowCount() const {
    return rowHeights.size();
}

void TextureGrid::rebuildCells() {
    grid.clear();
    float v = 0;
    for (size_t j = 0; j < rowHeights.size(); ++j) {
        float u = 0;
        for (size_t i = 0; i < columnWidths.size(); ++i) {
            grid.push_back({
                u, v,
                columnWidths[i], rowHeights[j],
                StretchMode::Fixed, StretchMode::Fixed
                });
            u += columnWidths[i];
        }
        v += rowHeights[j];
    }
}

bool TextureGrid::isStretchCol(size_t i) const {
    for (size_t j = 0; j < rowHeights.size(); ++j) {
        if (grid[j * columnWidths.size() + i].modeX == StretchMode::Stretch)
            return true;
    }
    return false;
}

bool TextureGrid::isStretchRow(size_t j) const {
    for (size_t i = 0; i < columnWidths.size(); ++i) {
        if (grid[j * columnWidths.size() + i].modeY == StretchMode::Stretch)
            return true;
    }
    return false;
}

void TextureGrid::draw(Texture& texture, Game& game, float x, float y, float w, float h, float w1, float h1, glmath::vec4 const & color) {
    UseTexture bind(game.getWindow(), texture);

    const size_t cols = columnWidths.size();
    const size_t rows = rowHeights.size();

    float scaleX = w / w1;
    float scaleY = h / h1;

    auto computeScaled = [&](const std::vector<float>& sizes, const std::vector<bool>& stretchFlags, float totalTarget, float totalNorm) {
            std::vector<float> result(sizes.size());
            auto fixedSumNorm = 0.0f;
            auto stretchSumNorm = 0.0f;
            for (size_t i = 0; i < sizes.size(); ++i) {
                if (stretchFlags[i]) 
                    stretchSumNorm += sizes[i];
                else 
                    fixedSumNorm += sizes[i];
            }

            auto const scaledFixedTarget = fixedSumNorm * totalNorm;
            auto const remainingTarget = totalTarget - scaledFixedTarget;
            for (size_t i = 0; i < sizes.size(); ++i) {
                if (stretchFlags[i])
                    result[i] = sizes[i] * remainingTarget / stretchSumNorm;
                else
                    result[i] = sizes[i] * totalNorm;
            }
            return result;
        };

    auto stretchColumns = std::vector<bool>{};
    auto stretchRows = std::vector<bool>{};
    for (auto i = 0; i < getColumnCount(); ++i)
        stretchColumns.push_back(isStretchCol(i));
    for (auto i = 0; i < getRowCount(); ++i)
        stretchRows.push_back(isStretchRow(i));

    auto const scaledColumnWidths = computeScaled(columnWidths, stretchColumns, w, w1);
    auto const scaledRowHeights = computeScaled(rowHeights, stretchRows, h, h1);
    std::vector<float> uCoords(cols + 1, 0.0f);
    std::vector<float> vCoords(rows + 1, 0.0f);
    for (size_t i = 0; i < cols; ++i)
        uCoords[i + 1] = uCoords[i] + columnWidths[i];
    for (size_t j = 0; j < rows; ++j)
        vCoords[j + 1] = vCoords[j] + rowHeights[j];

    std::vector<float> xCoords(cols + 1, 0.0f);
    std::vector<float> yCoords(rows + 1, 0.0f);
    xCoords[0] = x;
    yCoords[0] = y;
    for (size_t i = 0; i < cols; ++i)
        xCoords[i + 1] = xCoords[i] + scaledColumnWidths[i];
    for (size_t j = 0; j < rows; ++j)
        yCoords[j + 1] = yCoords[j] + scaledRowHeights[j];

    auto va = glutil::VertexArray{};
    const float nx = 0.0f, ny = 1.0f, nz = 0.0f; // je nach Konvention vielleicht (0,1,0)
    for (size_t j = 0; j < rows; ++j) {
        for (size_t i = 0; i < cols; ++i) {
            float x0 = xCoords[i];
            float x1 = xCoords[i + 1];
            float y0 = yCoords[j];
            float y1 = yCoords[j + 1];

            float u0 = uCoords[i];
            float u1 = uCoords[i + 1];
            float v0 = vCoords[j];
            float v1 = vCoords[j + 1];

            va.normal(nx, ny, nz).color(color).texCoord(u0, v0).vertex(x0, y0);
            va.normal(nx, ny, nz).color(color).texCoord(u0, v1).vertex(x0, y1);
            va.normal(nx, ny, nz).color(color).texCoord(u1, v0).vertex(x1, y0);

            va.normal(nx, ny, nz).color(color).texCoord(u1, v0).vertex(x1, y0);
            va.normal(nx, ny, nz).color(color).texCoord(u0, v1).vertex(x0, y1);
            va.normal(nx, ny, nz).color(color).texCoord(u1, v1).vertex(x1, y1);
        }
    }

    va.draw(GL_TRIANGLES);

    //struct Vertex {
    //    float x, y, z;
    //    float u, v;
    //};

    //std::vector<Vertex> vertices;
    //vertices.reserve((cols + 1) * (rows + 1));

    //for (size_t j = 0; j <= rows; ++j) {
    //    for (size_t i = 0; i <= cols; ++i) {
    //        vertices.push_back({
    //            xCoords[i],
    //            yCoords[j],
    //            0.0f,
    //            uCoords[i],
    //            vCoords[j]
    //            });
    //    }
    //}

    //std::vector<unsigned int> indices;
    //indices.reserve(rows * cols * 6);

    //for (size_t j = 0; j < rows; ++j) {
    //    for (size_t i = 0; i < cols; ++i) {
    //        unsigned int topLeft = j * (cols + 1) + i;
    //        unsigned int topRight = j * (cols + 1) + i + 1;
    //        unsigned int bottomLeft = (j + 1) * (cols + 1) + i;
    //        unsigned int bottomRight = (j + 1) * (cols + 1) + i + 1;

    //        indices.insert(indices.end(), {
    //            topLeft, bottomLeft, topRight,
    //            topRight, bottomLeft, bottomRight
    //            });
    //    }
    //}

    //IndexedVertexBuffer va;

    //for (auto& v : vertices) {
    //    va.normal(0, 1, 0)
    //        .color(color)
    //        .texCoord(v.u, v.v)
    //        .vertex(v.x, v.y);
    //}

    //va.upload();
    //va.draw(GL_TRIANGLES);
}
