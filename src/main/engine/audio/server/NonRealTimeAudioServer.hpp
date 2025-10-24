#pragma once

#include <engine/audio/server/AudioServer.hpp>
#include <engine/audio/server/AudioClient.hpp>

#include <thread>

#include <string>
#include <vector>

namespace mpc::engine::audio::server
{

    class NonRealTimeAudioServer : public AudioServer, public AudioClient
    {

    private:
        std::shared_ptr<NonRealTimeAudioServer> me;

        bool realTime = true;
        bool isRunningNonRealTime = false;
        std::shared_ptr<AudioServer> server;
        std::shared_ptr<AudioClient> client;
        std::thread nonRealTimeThread;

        void runNonRealTime();

        void startNonRealTimeThread();

        void stopNonRealTimeThread();

    public:
        void setRealTime(bool rt);

        bool isRealTime();

        void setSharedPtr(std::shared_ptr<NonRealTimeAudioServer> sharedPtr);

        void start() override;

        void stop() override;

        void close() override;

        void setClient(std::shared_ptr<AudioClient> client) override;

        bool isRunning() override;

        mpc::engine::audio::core::AudioBuffer *createAudioBuffer(std::string name) override;

        void removeAudioBuffer(mpc::engine::audio::core::AudioBuffer *buffer) override;

        IOAudioProcess *openAudioOutput(std::string name) override;

        IOAudioProcess *openAudioInput(std::string name) override;

        void closeAudioOutput(IOAudioProcess *output) override;

        void closeAudioInput(IOAudioProcess *input) override;

        float getSampleRate() override;

        void setSampleRate(int rate) override;

        void resizeBuffers(int newSize) override;

        void work(int nFrames) override;
        void work(const float *const *inputBuffer,
                  float *const *outputBuffer,
                  const int nFrames,
                  const std::vector<int8_t> &mpcMonoInputChannelIndices,
                  const std::vector<int8_t> &mpcMonoOutputChannelIndices,
                  const std::vector<int8_t> &hostInputChannelIndices,
                  const std::vector<int8_t> &hostOutputChannelIndices);
        NonRealTimeAudioServer(std::shared_ptr<AudioServer> server);
        ~NonRealTimeAudioServer();
    };

} // namespace mpc::engine::audio::server
