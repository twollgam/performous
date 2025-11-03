#pragma once

#include "render_params.hh"
#include "note.hh"

#include <memory>
#include <string>

namespace Guitar {

    class IRenderer {
    public:
        virtual ~IRenderer() = default;

        // Called once when renderer should init GL resources
        virtual void init() = 0;

        // Draw the static fretboard (strings, fret markers)
        virtual void drawFretboard(const Guitar::Note& note, const RenderParams& params) = 0;

        // Draw notes (current and future)
        virtual void drawNotes(double time, const std::vector<Guitar::Note>& note, const Guitar::RenderParams& params) = 0;

        // Draw feedback text (e.g. note names, combos)
        virtual void drawOverlayText(const std::string& text, float x, float y) = 0;

        // Called each frame to present / swap buffers if the renderer owns the
        // context. Often this is no-op if your app handles swap.
        virtual void present() = 0;
    };

    using Renderer = std::shared_ptr<IRenderer>;

}
