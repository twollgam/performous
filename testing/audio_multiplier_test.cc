#include <gtest/gtest.h>
#include <gmock/gmock.h>

#include "game/audio/multiplier.hh"

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
            for (auto i = 0U; i < _n; ++i)
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
}

TEST(UnitTest_Multiplier, factor_1)
{
    auto sut = std::make_shared<Multiplier>();
    auto source = std::make_shared<Source>();
    auto sink = std::make_shared<Sink>();

    source->connect(sut);
    sut->connect(sink);
    source->push();

    EXPECT_EQ(10, sink->m_data.size());

    for (auto i = 0U; i < sink->m_data.size(); ++i)
        EXPECT_FLOAT_EQ(float(i), sink->m_data[i]);
}

TEST(UnitTest_Multiplier, factor_2)
{
    auto sut = std::make_shared<Multiplier>();
    auto source = std::make_shared<Source>();
    auto sink = std::make_shared<Sink>();

    sut->setFactor(2);

    source->connect(sut);
    sut->connect(sink);
    source->push();

    EXPECT_EQ(10, sink->m_data.size());

    for (auto i = 0U; i < sink->m_data.size(); ++i)
        EXPECT_FLOAT_EQ(float(i) * 2.0f, sink->m_data[i]);
}

TEST(UnitTest_Multiplier, factor_0_5)
{
    auto sut = std::make_shared<Multiplier>();
    auto source = std::make_shared<Source>();
    auto sink = std::make_shared<Sink>();

    sut->setFactor(0.5);

    source->connect(sut);
    sut->connect(sink);
    source->push();

    EXPECT_EQ(10, sink->m_data.size());

    for (auto i = 0U; i < sink->m_data.size(); ++i)
        EXPECT_FLOAT_EQ(float(i) * 0.5f, sink->m_data[i]);
}

TEST(UnitTest_Multiplier, factor_0_5_iterations_3)
{
    auto sut = std::make_shared<Multiplier>();
    auto source = std::make_shared<Source>();
    auto sink = std::make_shared<Sink>();

    sut->setFactor(0.5);

    source->connect(sut);
    sut->connect(sink);
    source->push();

    for (auto n = 0; n < 3; ++n) {
        EXPECT_EQ(10, sink->m_data.size());

        for (auto i = 0U; i < sink->m_data.size(); ++i)
            EXPECT_FLOAT_EQ(float(i) * 0.5f, sink->m_data[i]);
    }
}


