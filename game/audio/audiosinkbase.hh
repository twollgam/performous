#pragma once

#include "iaudiosink.hh"
#include "iaudiosource.hh"

#include <vector>
#include <memory>

namespace audio {
    struct AudioSinkBase : public IAudioSink {
        virtual void link(AudioSourcePtr const& source);
        
        virtual void pull();

    private:
        AudioSourcePtr m_source;
    };
}
