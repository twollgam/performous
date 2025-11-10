#pragma once

#include "note_type.hh"
#include "fingering.hh"
#include "strumming_pattern.hh"

#include <vector>

namespace Guitar {

    struct Note {
        std::string chord;
        Fingering fingering;
        NoteType type = NoteType::Chord;
        StrummingPatterns patterns;
        float time = 0.f;         // absolute playback time (seconds)
        float duration = 0.f;     // seconds (for sustained notes)
        int id = 0;               // optional id

        bool operator<(const Note& other) const { return time < other.time; }
        bool isActive(double time) const { return time > this->time && time < this->time + duration; }
    };

}
