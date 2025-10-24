#pragma once
#include <engine/audio/server/AudioClient.hpp>
#include <engine/audio/server/IOAudioProcess.hpp>
#include <engine/audio/core/AudioBuffer.hpp>

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

namespace mpc::engine::audio::server
{

    class AudioServer
    {

    protected:
        float sampleRate{44100.0};
        unsigned int bufferSize{512};
        AudioClient *client{nullptr};
        bool running{false};
        std::vector<IOAudioProcess *> activeInputs;
        std::vector<IOAudioProcess *> activeOutputs;
        std::vector<mpc::engine::audio::core::AudioBuffer *> buffers;

    public:
        virtual void resizeBuffers(int newSize);
        virtual mpc::engine::audio::core::AudioBuffer *createAudioBuffer(std::string name);
        virtual void removeAudioBuffer(mpc::engine::audio::core::AudioBuffer *buffer);

    public:
        const std::vector<mpc::engine::audio::core::AudioBuffer *> &getBuffers();
        const unsigned int getBufferSize();

    public:
        virtual void start() = 0;
        virtual void stop() = 0;
        virtual bool isRunning() = 0;
        virtual void close() = 0;
        virtual void setClient(std::shared_ptr<AudioClient> client) = 0;
        virtual IOAudioProcess *openAudioOutput(std::string name) = 0;
        virtual IOAudioProcess *openAudioInput(std::string name) = 0;
        virtual void closeAudioOutput(IOAudioProcess *output) = 0;
        virtual void closeAudioInput(IOAudioProcess *input) = 0;
        virtual float getSampleRate();
        virtual void setSampleRate(int rate);

    public:
        AudioServer();
        ~AudioServer();

    private:
        friend class StereoInputProcess;
        friend class StereoOutputProcess;
    };
} // namespace mpc::engine::audio::server
