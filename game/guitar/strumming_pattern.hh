#pragma once

#include "stroke_type.hh"

#include <map>

struct StrummingPattern {
    std::string name;
    int difficulty;
    Strokes pattern;
    std::string comment;
};

struct StrummingPatterns {
    std::string name;
    std::map<int, StrummingPattern> patterns;
};

