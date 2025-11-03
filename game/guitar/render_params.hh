#pragma once

#include "graphic/glmath.hh"

#include <array>

namespace Guitar {

    struct RenderParams {
        int visibleFrets = 20;        // how many frets are visible in the scene
        float fretSpacing = 0.05f;    // logical units per fret (tweak as needed)
        float noteSpeed = 2.0f;       // logical units per second (how fast notes move)
        std::array<glmath::vec4, 6> stringColors{
            glmath::vec4{ 1.0f, 0.0f, 0.0f, 1.f }, // E
            glmath::vec4{ 1.0f, 0.5f, 0.0f, 1.f }, // A
            glmath::vec4{ 1.0f, 1.0f, 0.0f, 1.f }, // D
            glmath::vec4{ 0.0f, 0.0f, 1.0f, 1.f }, // G
            glmath::vec4{ 0.0f, 1.0f, 0.0f, 1.f }, // B
            glmath::vec4{ 0.5f, 0.0f, 1.0f, 1.f }  // e
        };
    };
}
