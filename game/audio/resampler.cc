#include "resampler.hh"

#include <cmath>
#include <iostream>

namespace audio {
    void Resampler::setRate(float rate) {
        m_rate = rate;
    }
    
    void Resampler::setRate(unsigned in, unsigned out) {
        setRate(float(in) / float(out));
    }

    void Resampler::write(std::vector<float>& data) {
        m_data.swap(data);
        
        push();
    }

    void Resampler::read(std::vector<float>& data) {
        if(m_data.empty())
            return;
        if(m_rate == 1.0f)
            for(auto i = 0U; i < m_data.size(); ++i)
                data.emplace_back(m_data[i]);
        else {
            const auto outLength = unsigned(float(m_data.size()) * (1.0f / m_rate));
            const auto step = float(m_data.size() - 1) / float(outLength - 1);
            for(auto i = 0U; i < outLength; ++i) {
                const auto inPosition = float(i) * step;
                const auto inPositionA = unsigned(floor(inPosition));
                const auto inPositionB = inPositionA + 1;
                const auto delta = inPosition - float(inPositionA);
                const auto left = 1.0 - delta;
                const auto right = delta;
                const auto value = m_data[inPositionA] * left + m_data[inPositionB] * right;
                std::cout << inPositionA << " " << inPositionB << " " << value << std::endl; 
                data.emplace_back(value);
            }
        }
    }
        
    size_t Resampler::getAvailableSamples() const {
        return m_data.size();
    }   
}
