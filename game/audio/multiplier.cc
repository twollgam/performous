#include "multiplier.hh"

#include <cmath>
#include <iostream>

namespace audio {
    void Multiplier::setFactor(float factor) {
        m_factor = factor;
    }

    void Multiplier::write(std::vector<float>& data) {
        m_data.swap(data);

        push();
    }

    void Multiplier::read(std::vector<float>& data) {
        if (m_data.empty())
            return;
    
        if (m_factor != 1.0f) {
            for (auto i = 0U; i < m_data.size(); ++i)
                m_data[i] *= m_factor;
        }

        data.swap(m_data);
    }

    size_t Multiplier::getAvailableSamples() const {
        return m_data.size();
    }
}
