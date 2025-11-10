#pragma once

#include <string>
#include <vector>

enum class StrokeType {
    Pause,
    Down,
    Up,
    StringA,
    StringD,
    StringG,
    StringB,
    StringE,
    StringHighE,
    Ghost,
    PalmMute,
    Unknown
};

using Strokes = std::vector<StrokeType>;

Strokes parseStrokes(const std::string&);
