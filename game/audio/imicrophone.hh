#pragma once

#include "iaudiosource.hh"
#include "audiosourcebase.hh"

#include <memory>
#include <string>

namespace audio {
    struct IMicrophone : public AudioSourceBase {
        virtual ~IMicrophone() = default;
        
        virtual std::string getName() const = 0;
    };

    using MicrophonePtr = std::shared_ptr<IMicrophone>;
}
