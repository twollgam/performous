#include "audiosourcebase.hh"
#include "audiosinkbase.hh"

namespace audio {
    void AudioSourceBase::connect(AudioSinkBasePtr const& sink) {
        m_sink = sink;
    }

    void AudioSourceBase::push() {
        if(m_sink && getAvailableSamples() > 0) {
            auto buffer = std::vector<float>();
            
            read(buffer);
            
            m_sink->write(buffer);
        }
    }
} 
