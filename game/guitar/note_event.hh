#pragma once

#include "note.hh"

namespace Guitar {

    struct NoteEvent {
        const Note note;
        float hitTimeOffset; // seconds: negative if early, positive if late
        bool hit;            // true when hit, false when missed
    };
}
