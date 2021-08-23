#pragma once

#include <vector>
#include <memory>

namespace audio {
    struct IAudioSource {
        virtual  ~IAudioSource() = default;
        
        virtual void start() = 0;
        virtual void stop() = 0;
        
        virtual size_t getAvailableSamples() const = 0;
        virtual void read(std::vector<float>&) = 0;
    };

    using AudioSourcePtr = std::shared_ptr<IAudioSource>;
}
