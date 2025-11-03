#pragma once

#include "irenderer.hh"
#include "note.hh"
#include "note_event.hh"

#include <functional>
#include <mutex>
#include <queue>

class FretboardRenderer {
public:
    FretboardRenderer();

    void setRenderer(Guitar::Renderer r) { renderer = r; }

    // Configure rendering / timings
    void setRenderParams(const Guitar::RenderParams& p) { params = p; }

    // Push a new incoming note. The Fretboard assumes note.time is absolute
    // playback time (seconds since some epoch matching "now()" used in update).
    void pushNote(const Guitar::Note& n);

    // Call every frame to update state. `now` is absolute time in seconds.
    // Use your game's main clock (e.g. audio playback time) to keep notes
    // synchronized with audio. Returns the number of processed note events
    // (hit/miss) during this update.
    int update(double now);

    // Render the current visual state. `now` is used to compute note positions.
    void render(double now);

    // Hit detection: called by input system when player attempts to play a note.
    // It returns true if a matching note was hit within the tolerance window.
    bool tryHit(int stringIndex, int fretIndex, double now);

    // Callback when user hits or misses a note
    std::function<void(const Guitar::NoteEvent&)> onNoteEvent;

    // Tolerance window in seconds for a note to be considered 'hit'
    float hitWindow = 0.15f; // +/- 150 ms

    // Time offset to apply from audio clock (e.g. sync adjustments)
    double timeOffset = 0.0;

private:
    Guitar::Renderer renderer;
    Guitar::RenderParams params;

    using Note = Guitar::Note;

    // A min-heap or sorted queue of upcoming notes
    std::mutex notesMutex;
    std::priority_queue<Note, std::vector<Note>, std::function<bool(const Note&, const Note&)>> notes{
        [](const Note& a, const Note& b) { return a.time > b.time; } // smallest time first
    };

    // Active notes currently visible on the fretboard (in time window)
    std::vector<Note> activeNotes;

    // Called internally to move notes from notes (queue) into activeNotes when they
    // are within visibility range.
    void populateActive(double now);

    // Helper: find an active note matching string/fret within hitWindow
    // If matched, mark it removed from active list and return it.
    bool findAndRemoveMatching(int stringIndex, int fretIndex, double now, Note& outNote, float& offsetOut);
};

