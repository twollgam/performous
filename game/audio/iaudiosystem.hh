#include "imicrophone.hh"

#include <memory>
#include <string>
#include <vector>

namespace audio {
    struct IAudioSystem {
        virtual ~IAudioSystem() = default;
        
        virtual std::string getName() const = 0;
        
        virtual std::vector<MicrophonePtr> getMicrophones() const = 0;
        //virtual std::vector<AudioSinkPtr> getAudioSinks() const = 0;
    };

    using AudioSystemPtr = std::shared_ptr<IAudioSystem>;
}
