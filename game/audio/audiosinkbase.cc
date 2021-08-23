#include "audiosinkbase.hh"
#include "audiosourcebase.hh"

namespace audio {
    void AudioSinkBase::link(AudioSourcePtr const& source) {
        m_source = source;
    }
        
    void AudioSinkBase::pull() {
        if(m_source && m_source->getAvailableSamples() > 0) {
            auto buffer = std::vector<float>();
            
            m_source->read(buffer);
            
            write(buffer);
        }
    }
}
