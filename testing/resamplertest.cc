#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "game/audio/resampler.hh"

using namespace audio;

namespace {
    struct Sink : public AudioSinkBase {
        void write(std::vector<float>& data) override {
            m_data.swap(data);
        }
        
        std::vector<float> m_data;
    };
    
    struct Source : public AudioSourceBase {
        Source(unsigned n = 10)
        : _n(n)
        {
        }
        
        void read(std::vector<float>& data) override {
            for(auto i = 0U; i < _n; ++i)
                data.emplace_back(i);
        }
        
        void start() override {}
        void stop() override {}
        
        size_t getAvailableSamples() const override {
            return 10;
        }
        
    private:
        unsigned _n;
    };
    
    
    TEST(UnitTest_Resampler, rate_1)
    {
        auto resampler = std::make_shared<Resampler>();
        auto source = std::make_shared<Source>();
        auto sink = std::make_shared<Sink>();
        
        source->connect(resampler);
        resampler->connect(sink);
        source->push();
   
        EXPECT_EQ(10, sink->m_data.size());
        
        for(auto i = 0U; i < sink->m_data.size(); ++i)
            EXPECT_FLOAT_EQ(float(i), sink->m_data[i]);
    }
    
    TEST(UnitTest_Resampler, rate_2)
    {
        auto resampler = std::make_shared<Resampler>();
        auto source = std::make_shared<Source>();
        auto sink = std::make_shared<Sink>();
        
        resampler->setRate(2);
        
        source->connect(resampler);
        resampler->connect(sink);
        source->push();
   
        EXPECT_EQ(5, sink->m_data.size());
        
        for(auto i = 0U; i < sink->m_data.size(); ++i)
            EXPECT_FLOAT_EQ(float(i) * 9.0 / 4.0, sink->m_data[i]);
    }
    
    TEST(UnitTest_Resampler, rate_0_5)
    {
        auto resampler = std::make_shared<Resampler>();
        auto source = std::make_shared<Source>();
        auto sink = std::make_shared<Sink>();
        
        resampler->setRate(0.5);

        source->connect(resampler);
        resampler->connect(sink);
        source->push();
   
        EXPECT_EQ(20, sink->m_data.size());
        
        for(auto i = 0U; i < sink->m_data.size(); ++i)
            EXPECT_FLOAT_EQ(float(i) * 9.0 / 19.0, sink->m_data[i]);
    }
    
    TEST(UnitTest_Resampler, rate_44100_48000)
    {
        auto resampler = std::make_shared<Resampler>();
        auto source = std::make_shared<Source>(100);
        auto sink = std::make_shared<Sink>();
        
        resampler->setRate(44100, 48000);

        source->connect(resampler);
        resampler->connect(sink);
        source->push();
   
        EXPECT_EQ(108, sink->m_data.size());
        
        for(auto i = 0U; i < sink->m_data.size(); ++i)
            EXPECT_FLOAT_EQ(float(i) * 99.0 / 107.0, sink->m_data[i]);
    }
    
    TEST(UnitTest_Resampler, rate_48000_44100)
    {
        auto resampler = std::make_shared<Resampler>();
        auto source = std::make_shared<Source>(100);
        auto sink = std::make_shared<Sink>();
        
        resampler->setRate(48000, 44100);

        source->connect(resampler);
        resampler->connect(sink);
        source->push();
   
        EXPECT_EQ(91, sink->m_data.size());
        
        for(auto i = 0U; i < sink->m_data.size(); ++i)
            EXPECT_FLOAT_EQ(float(i) * 99.0 / 90.0, sink->m_data[i]);
    }
}
 

