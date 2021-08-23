#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "game/portaudio.hh"
#include "game/configuration.hh"
#include "game/audio/audiosinkbase.hh"

#include <thread>

using namespace audio;

namespace {
    TEST(IntegrationTest_PortAudio, constructor)
    {
        EXPECT_NO_THROW(PortAudio());
    }

    TEST(IntegrationTest_PortAudio, getMicrophones)
    {
        config["audio/latency"] = 0.0f;
        const auto system = PortAudio();
        const auto microphones = system.getMicrophones();
        
        EXPECT_FALSE(microphones.empty());
        
        auto n = 1;
        for(const auto& microphone : microphones)
            std::cout << n++ << ": " << microphone->getName() << std::endl;
    }

    TEST(IntegrationTest_PortAudio, microphone_start_stop)
    {
        config["audio/latency"] = 0.0f;
        const auto system = PortAudio();
        const auto microphones = system.getMicrophones();
        
        ASSERT_FALSE(microphones.empty());
        
        auto microphone = microphones.back();

        microphone->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        microphone->stop();
    
        EXPECT_LT(1000, microphone->getAvailableSamples());
    }

    struct Sink : public AudioSinkBase {
        void write(std::vector<float>& data) override {
            m_count += data.size();
        }
        
        size_t m_count = 0;
    };
    
    TEST(IntegrationTest_PortAudio, microphone_start_stop_push)
    {
        config["audio/latency"] = 0.0f;
        const auto system = PortAudio();
        const auto microphones = system.getMicrophones();
        
        ASSERT_FALSE(microphones.empty());
        
        auto microphone = microphones.back();
        auto sink = std::make_shared<Sink>();
        
        microphone->connect(sink);
        microphone->start();
        std::this_thread::sleep_for(std::chrono::milliseconds(100));
        microphone->stop();
    
        EXPECT_EQ(0, microphone->getAvailableSamples());
        EXPECT_LT(1000, sink->m_count);
    }
}
