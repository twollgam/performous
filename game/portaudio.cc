#include "portaudio.hh"

#include "libda/portaudio.hpp"
#include "configuration.hh"

#include <thread>

namespace {
    auto init = std::shared_ptr<portaudio::Init>(); 
    
    struct ScopedLock {
        ScopedLock(std::atomic_bool& lock)
        : m_lock(lock) {
            while(m_lock.exchange(true))
                std::this_thread::sleep_for(std::chrono::milliseconds(10));
        }
        ~ScopedLock() {
            m_lock = false;
        }
        
    private:
        std::atomic_bool& m_lock;
    };
    
    class PortAudioMicrophone : public audio::IMicrophone {
    public:
        PortAudioMicrophone(portaudio::DeviceInfo& info, size_t n)
        : m_name(info.name + " (" + std::to_string(n) + "/" + std::to_string(info.in) + ")"), 
          m_device(info), 
          m_n(n)
        {
        }

        void start() override {
            m_stream = std::make_unique<portaudio::Stream>(
                *this, 
                portaudio::Params()
                    .channelCount(1)
                    .device(m_device.index)
                    .suggestedLatency(config["audio/latency"].f()), 
                nullptr, 
                48000, m_device.index);
            
            auto error = Pa_StartStream(*m_stream);
            if (error != paNoError) 
                throw std::runtime_error(std::string("Pa_StartStream: ") + Pa_GetErrorText(error));
        }
        void stop() override {
            const auto lock = ScopedLock(m_lock);            
            //const auto error = Pa_StopStream(*m_stream);
            //if (error != paNoError)
            //    throw std::runtime_error(std::string("Pa_StopStream: ") + Pa_GetErrorText(error));
            
            m_stream.reset();
        }

        std::string getName() const override { 
            return m_name; 
        }
        size_t getAvailableSamples() const override {
            return m_samples.size();
        }
        void read(std::vector<float>& samples) override {
            const auto lock = ScopedLock(m_lock);
            
            samples.swap(m_samples);
            m_samples.clear();
        }

        int operator()(float const* input, float*, unsigned long frames) {
            {
                const auto lock = ScopedLock(m_lock);
                            
                for(auto i = decltype(frames)(0); i < frames; ++i)
                    m_samples.push_back(input[i]);
            }
            
            push();
            
            return paContinue;
        }

    private:
        
    private:
        const std::string m_name;
        portaudio::DeviceInfo m_device;
        size_t m_n;
        std::unique_ptr<portaudio::Stream> m_stream;
        std::vector<float> m_samples;
        std::atomic_bool m_lock{false};
    };
}

PortAudio::PortAudio()
: m_init(init) {
    if(!m_init)
        init = m_init = std::make_shared<portaudio::Init>();
}

std::string PortAudio::getName() const {
    return "Port Audio";
}

std::vector<audio::MicrophonePtr> PortAudio::getMicrophones() const {
    auto&& devices = portaudio::AudioDevices();
    auto result = std::vector<audio::MicrophonePtr>();
    
    for(auto& device : devices.devices) {
        std::cout << "port audio device: " << device.name << std::endl;
        if(device.out > 0 || device.in > 2)
            continue;
        for(auto n = decltype(device.in)(0); n < device.in; ++n)
            result.emplace_back(std::make_shared<PortAudioMicrophone>(device, n));
    }
    
    return result;
}
