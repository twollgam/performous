#pragma once

#include "audiosinkbase.hh"
#include "audiosourcebase.hh"

namespace audio {
    class Multiplier : public AudioSinkBase, public AudioSourceBase
    {
    public:
        void setFactor(float);

        void write(std::vector<float>& data) override;
        void read(std::vector<float>& data) override;

        void start() override {}
        void stop() override {}

        size_t getAvailableSamples() const override;

    private:
        std::vector<float> m_data;
        float m_factor = 1.0f;
    };
}
