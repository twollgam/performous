#pragma once

#include "audio/iaudiosystem.hh"

namespace portaudio {
    struct Init;
};

class PortAudio : public audio::IAudioSystem {
public:
    PortAudio();
    ~PortAudio() override = default;
    
    std::string getName() const override;
    
    std::vector<audio::MicrophonePtr> getMicrophones() const override;
    
private:
    std::shared_ptr<portaudio::Init> m_init;
};
