#pragma once

#include <vector>
#include <memory>

namespace audio {
    struct IAudioSink {
        virtual  ~IAudioSink() = default;
        
        virtual void write(std::vector<float>&) = 0;
    };

    using AudioSinkPtr = std::shared_ptr<IAudioSink>;
}
