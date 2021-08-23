#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "game/audio/audiosourcebase.hh"
#include "game/audio/audiosinkbase.hh"

#include <thread>

using namespace audio;

namespace {
    struct Sink : public AudioSinkBase {
        void write(std::vector<float>& data) override {
            m_data.swap(data);
        }
        
        std::vector<float> m_data;
    };
    
    struct Source : public AudioSourceBase {
        void read(std::vector<float>& data) override {
            for(auto i = 0; i < 10; ++i)
                data.emplace_back(i);
        }
        
        void start() override {}
        void stop() override {}
        
        size_t getAvailableSamples() const override {
            return 10;
        }
    };
    
    struct Filter : public AudioSourceBase, public AudioSinkBase {
        void write(std::vector<float>& data) override {
            m_data.swap(data);
            
            push();
        }
        void read(std::vector<float>& data) override {
            for(auto i = 0U; i < m_data.size(); ++i)
                data.emplace_back(m_data[i] * 2);
        }
        
        void start() override {}
        void stop() override {}
        
        size_t getAvailableSamples() const override {
            return m_data.size();
        }

        std::vector<float> m_data;
    };
    
    struct Connector : public AudioSourceBase, public AudioSinkBase {
        void write(std::vector<float>& data) override {
            m_data.swap(data);
            
            push();
        }
        void read(std::vector<float>& data) override {
            for(auto i = 0U; i < m_data.size(); ++i)
                data.emplace_back(m_data[i]);
        }
        void connect(AudioSinkBasePtr const& sink) override {
            m_sinks.emplace_back(sink);
        }
        void push() override {
            if(getAvailableSamples() > 0) {
                auto buffer = std::vector<float>();
                
                read(buffer);
                
                for(auto sink : m_sinks) {
                    auto copy = std::vector<float>(buffer);
                    
                    sink->write(copy);
                }
            }
        }

        void start() override {}
        void stop() override {}
        
        size_t getAvailableSamples() const override {
            return m_data.size();
        }

        std::vector<float> m_data;
        std::vector<AudioSinkBasePtr> m_sinks;
    };
    
    TEST(UnitTest_AudioSourceSink, source_sink)
    {
        auto source = std::make_shared<Source>();
        auto sink = std::make_shared<Sink>();
        
        source->connect(sink);
        source->push();
   
        EXPECT_EQ(10, sink->m_data.size());
        
        for(auto i = 0U; i < sink->m_data.size(); ++i)
            EXPECT_EQ(float(i), sink->m_data[i]);
    }
    
    TEST(UnitTest_AudioSourceSink, source_sink_filter)
    {
        auto source = std::make_shared<Source>();
        auto sink = std::make_shared<Sink>();
        auto filter = std::make_shared<Filter>();
        
        source->connect(filter);
        filter->connect(sink);
        source->push();
   
        EXPECT_EQ(10, sink->m_data.size());
        
        for(auto i = 0U; i < sink->m_data.size(); ++i)
            EXPECT_EQ(float(i * 2), sink->m_data[i]);
    }
    
    TEST(UnitTest_AudioSourceSink, source_sink_filter2)
    {
        auto source = std::make_shared<Source>();
        auto sink = std::make_shared<Sink>();
        auto filterA = std::make_shared<Filter>();
        auto filterB = std::make_shared<Filter>();
        
        source->connect(filterA);
        filterA->connect(filterB);
        filterB->connect(sink);
        source->push();
   
        EXPECT_EQ(10, sink->m_data.size());
        
        for(auto i = 0U; i < sink->m_data.size(); ++i)
            EXPECT_EQ(float(i * 4), sink->m_data[i]);
    }
    
    TEST(UnitTest_AudioSourceSink, source_sink_connector)
    {
        auto source = std::make_shared<Source>();
        auto sinkA = std::make_shared<Sink>();
        auto sinkB = std::make_shared<Sink>();
        auto connector = std::make_shared<Connector>();
        
        source->connect(connector);
        connector->connect(sinkA);
        connector->connect(sinkB);
        source->push();
   
        EXPECT_EQ(10, sinkA->m_data.size());
        
        for(auto i = 0U; i < sinkA->m_data.size(); ++i)
            EXPECT_EQ(float(i), sinkA->m_data[i]);
   
        EXPECT_EQ(10, sinkB->m_data.size());
        
        for(auto i = 0U; i < sinkB->m_data.size(); ++i)
            EXPECT_EQ(float(i), sinkB->m_data[i]);
    }
}
 
