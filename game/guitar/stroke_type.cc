#include "stroke_type.hh"

Strokes parseStrokes(const std::string& s) {
    auto strokes = Strokes();

    for (auto&& c : s) {
        switch (c) {
        case 'd': strokes.emplace_back(StrokeType::Down); break;
        case 'u': strokes.emplace_back(StrokeType::Up); break;
        case '-': strokes.emplace_back(StrokeType::Pause); break;
        case '&': strokes.emplace_back(StrokeType::Pause); break;
        case 'E': strokes.emplace_back(StrokeType::StringE); break;
        case 'A': strokes.emplace_back(StrokeType::StringA); break;
        case 'D': strokes.emplace_back(StrokeType::StringD); break;
        case 'G': strokes.emplace_back(StrokeType::StringG); break;
        case 'B': strokes.emplace_back(StrokeType::StringB); break;
        case 'e': strokes.emplace_back(StrokeType::StringHighE); break;
        case 'x': strokes.emplace_back(StrokeType::Ghost); break;
        case 'p': strokes.emplace_back(StrokeType::PalmMute); break;
        default: break;
        }
    }

    return strokes;
}
