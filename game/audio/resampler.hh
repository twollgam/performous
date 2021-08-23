#pragma once

#include "audiosinkbase.hh"
#include "audiosourcebase.hh"

namespace audio {
    class Resampler : public AudioSinkBase, public AudioSourceBase
    {
    public:
        void setRate(float rate);
        void setRate(unsigned in, unsigned out);
        
        void write(std::vector<float>& data) override;
        void read(std::vector<float>& data) override;
        
        void start() override {}
        void stop() override {}
        
        size_t getAvailableSamples() const override;
        
    private:
        std::vector<float> m_data;
        float m_rate = 1.0f;
    };
}
