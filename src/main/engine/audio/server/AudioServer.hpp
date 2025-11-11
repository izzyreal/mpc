#pragma once
#include "engine/audio/server/AudioClient.hpp"
#include "engine/audio/server/IOAudioProcess.hpp"
#include "engine/audio/core/AudioBuffer.hpp"

#include <string>
#include <vector>
#include <memory>
#include <atomic>

namespace mpc::engine::audio::server
{
    class AudioServer
    {
    protected:
        float sampleRate{44100.0};
        unsigned int bufferSize{512};
        std::atomic<bool> running{false};
        std::vector<std::shared_ptr<IOAudioProcess>> activeInputs;
        std::vector<std::shared_ptr<IOAudioProcess>> activeOutputs;
        std::vector<std::shared_ptr<core::AudioBuffer>> buffers;

    public:
        virtual void resizeBuffers(int newSize);
        virtual std::shared_ptr<core::AudioBuffer>
        createAudioBuffer(std::string name);
        virtual void removeAudioBuffer(std::shared_ptr<core::AudioBuffer>);

        const std::vector<std::shared_ptr<core::AudioBuffer>> &getBuffers();
        const unsigned int getBufferSize() const;

        virtual void start() = 0;
        virtual void stop() = 0;
        virtual bool isRunning() = 0;
        virtual void close() = 0;
        virtual void setClient(std::shared_ptr<AudioClient> client) = 0;
        virtual std::shared_ptr<IOAudioProcess>
        openAudioOutput(std::string name) = 0;
        virtual std::shared_ptr<IOAudioProcess>
        openAudioInput(std::string name) = 0;
        virtual void closeAudioOutput(std::shared_ptr<IOAudioProcess>) = 0;
        virtual void closeAudioInput(std::shared_ptr<IOAudioProcess>) = 0;
        virtual float getSampleRate();
        virtual void setSampleRate(int rate);

        AudioServer();

    private:
        friend class StereoInputProcess;
        friend class StereoOutputProcess;
    };
} // namespace mpc::engine::audio::server
