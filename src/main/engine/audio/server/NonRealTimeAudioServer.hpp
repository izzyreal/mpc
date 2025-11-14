#pragma once

#include "engine/audio/server/AudioServer.hpp"
#include "engine/audio/server/AudioClient.hpp"

#include <memory>
#include <thread>

#include <string>
#include <vector>
#include <atomic>

namespace mpc::engine::audio::server
{
    class NonRealTimeAudioServer : public AudioServer, public AudioClient
    {

        std::atomic<bool> realTime{true};
        std::atomic<bool> isRunningNonRealTime{false};
        std::shared_ptr<AudioServer> server;
        std::shared_ptr<AudioClient> client;
        std::thread nonRealTimeThread;

        void runNonRealTime();

        void startNonRealTimeThread();

        void stopNonRealTimeThread();

    public:
        void setRealTime(bool rt);

        bool isRealTime() const;

        void setSharedPtr(std::shared_ptr<NonRealTimeAudioServer> sharedPtr);

        void start() override;

        void stop() override;

        void close() override;

        void setClient(std::shared_ptr<AudioClient> clientToUse) override;

        bool isRunning() override;

        std::shared_ptr<core::AudioBuffer>
        createAudioBuffer(std::string name) override;

        void removeAudioBuffer(std::shared_ptr<core::AudioBuffer>) override;

        std::shared_ptr<IOAudioProcess>
        openAudioOutput(std::string name) override;

        std::shared_ptr<IOAudioProcess>
        openAudioInput(std::string name) override;

        void closeAudioOutput(std::shared_ptr<IOAudioProcess>) override;

        void closeAudioInput(std::shared_ptr<IOAudioProcess>) override;

        float getSampleRate() override;

        void setSampleRate(int rate) override;

        void resizeBuffers(int newSize) override;

        void work(int nFrames) override;
        void work(const float *const *inputBuffer, float *const *outputBuffer,
                  const int nFrames,
                  const std::vector<int8_t> &mpcMonoInputChannelIndices,
                  const std::vector<int8_t> &mpcMonoOutputChannelIndices,
                  const std::vector<int8_t> &hostInputChannelIndices,
                  const std::vector<int8_t> &hostOutputChannelIndices) const;
        NonRealTimeAudioServer(std::shared_ptr<AudioServer> serverToUse);
        ~NonRealTimeAudioServer() override;
    };

} // namespace mpc::engine::audio::server
