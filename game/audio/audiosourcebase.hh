#pragma once

#include "iaudiosource.hh"

#include <vector>
#include <memory>

namespace audio {
    struct AudioSinkBase;
    using AudioSinkBasePtr = std::shared_ptr<AudioSinkBase>;

    struct AudioSourceBase : public IAudioSource {
        virtual void connect(AudioSinkBasePtr const& sink);
        
        virtual void push();
        
    private:
        AudioSinkBasePtr m_sink;
    };
} 
