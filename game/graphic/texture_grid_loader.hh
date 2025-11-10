#pragma once

#include "texture_grid.hh"

#include <filesystem>

class TextureGridLoader {
public:
    static TextureGrid loadFromJson(const std::filesystem::path& path);

private:
    static StretchMode parseStretchMode(const std::string& s);
};
