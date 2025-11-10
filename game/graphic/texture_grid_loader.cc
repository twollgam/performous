#include "texture_grid_loader.hh"

#include <nlohmann/json.hpp>
#include <fstream>

TextureGrid TextureGridLoader::loadFromJson(const std::filesystem::path& path) {
    std::ifstream file(path);
    nlohmann::json j;
    file >> j;

    float texW = j["textureWidth"];
    float texH = j["textureHeight"];
    auto pxToU = [=](int px) { return px / texW; };
    auto pxToV = [=](int px) { return px / texH; };

    std::vector<float> cols;
    for (int c : j["columns"]) 
        cols.push_back(pxToU(c));
    std::vector<float> rows;
    for (int r : j["rows"]) 
        rows.push_back(pxToV(r));

    TextureGrid grid;
    grid.setGrid(cols, rows);

    if (j.contains("cells")) {
        for (auto& c : j["cells"]) {
            size_t col = c["col"];
            size_t row = c["row"];
            StretchMode modeX = parseStretchMode(c["modeX"]);
            StretchMode modeY = parseStretchMode(c["modeY"]);
            grid.setStretchMode(col, row, modeX, modeY);
        }
    }

    return grid;
}

StretchMode TextureGridLoader::parseStretchMode(const std::string& s) {
    if (s == "Fixed")
        return StretchMode::Fixed;
    if (s == "Stretch")
        return StretchMode::Stretch;
    if (s == "Tile")
        return StretchMode::Tile;
    throw std::invalid_argument("Unknown stretch mode: " + s);
}
