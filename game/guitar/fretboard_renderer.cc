#include "fretboard_renderer.hh"

#include <algorithm>
#include "note_event.hh"

FretboardRenderer::FretboardRenderer() {
}

void FretboardRenderer::pushNote(const Note& n) {
    std::lock_guard<std::mutex> g(notesMutex);
    notes.push(n);
}

void FretboardRenderer::populateActive(double now) {
    // The visible window: notes that will reach the hit line within the
    // 'lead' time determined by how many visible frets and noteSpeed.
    //float leadSeconds = (params.visibleFrets * params.fretSpacing) / params.noteSpeed;
    auto leadSeconds = 10.f;

    std::lock_guard<std::mutex> g(notesMutex);
    while (!notes.empty()) {
        const auto& note = notes.top();
        if (note.time <= now + leadSeconds) {
            activeNotes.push_back(note);
            notes.pop();
        }
        else 
            break;
    }
    // keep activeNotes sorted by time
    std::sort(activeNotes.begin(), activeNotes.end(), [](const Note& a, const Note& b) { return a.time < b.time; });
}

int FretboardRenderer::update(double now) {
    // populate active notes from the queue
    populateActive(now + timeOffset);

    // Check for missed notes: notes that passed beyond the hitWindow without being hit
    auto events = 0;
    for (auto it = activeNotes.begin(); it != activeNotes.end();) {
        double dt = now - it->time;
        if (dt > hitWindow) {
            // missed
            //NoteEvent ev{ *it, static_cast<float>(dt), false };
            //if (onNoteEvent) 
            //    onNoteEvent(ev);
            it = activeNotes.erase(it);
            ++events;
        }
        else {
            ++it;
        }
    }

    return events;
}

void FretboardRenderer::render(double time) {
    if (!renderer)
        return;

    auto const note = activeNotes.empty() || !activeNotes[0].isActive(time) ? Guitar::Note{} : activeNotes[0];

    renderer->drawFretboard(note, params);
    renderer->drawNotes(time, activeNotes, params);

    // Example overlay: active count
    renderer->drawOverlayText(std::to_string(activeNotes.size()) + " active", 10.f, 10.f);
    renderer->present();
}

bool FretboardRenderer::findAndRemoveMatching(int stringIndex, int fretIndex, double now, Note& outNote, float& offsetOut) {
    //// Find the closest note on the given string(s)
    //int bestIdx = -1;
    //float bestOffset = 1e6f;
    //for (size_t i = 0; i < activeNotes.size(); ++i) {
    //    const auto& n = activeNotes[i];
    //    // check whether this note contains the string
    //    auto contains = false;

    //    contains |= n.fingering.e_low
    //    for (int s : n.strings)
    //        if (s == stringIndex) { 
    //            contains = true; 
    //            break; 
    //        }
    //    if (!contains)
    //        continue;

    //    float dt = static_cast<float>(now - n.time);
    //    if (std::fabs(dt) <= hitWindow) {
    //        float absdt = std::fabs(dt);
    //        if (absdt < bestOffset) {
    //            bestOffset = absdt;
    //            bestIdx = static_cast<int>(i);
    //        }
    //    }
    //}

    //if (bestIdx >= 0) {
    //    outNote = activeNotes[bestIdx];
    //    offsetOut = (float)(now - outNote.time);
    //    activeNotes.erase(activeNotes.begin() + bestIdx);
    //    return true;
    //}
    return false;
}

bool FretboardRenderer::tryHit(int stringIndex, int fretIndex, double now) {
    Note matched;
    float offset;
    if (findAndRemoveMatching(stringIndex, fretIndex, now, matched, offset)) {
        Guitar::NoteEvent ev{ matched, offset, true };
        if (onNoteEvent)
            onNoteEvent(ev);
        return true;
    }
    return false;
}

