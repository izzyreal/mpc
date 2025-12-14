#pragma once

#include "engine/audio/server/AudioServer.hpp"

#include <string>
#include <vector>
#include <memory>
#include <cstdint>

/**
 * An AudioServer implementation that can be used with any externally provided
 * audio API.
 **/
namespace mpc::engine::audio::server
{
    class RealTimeAudioServer final : public AudioServer
    {
        std::shared_ptr<AudioClient> client;
        std::vector<int8_t> previousMpcMonoInputChannelIndices;

    public:
        RealTimeAudioServer();
        void start() override;
        void stop() override;
        bool isRunning() override;
        void close() override;
        void setClient(std::shared_ptr<AudioClient>) override;
        std::shared_ptr<IOAudioProcess>
        openAudioOutput(std::string name) override;
        std::shared_ptr<IOAudioProcess>
        openAudioInput(std::string name) override;
        void closeAudioOutput(std::shared_ptr<IOAudioProcess>) override;
        void closeAudioInput(std::shared_ptr<IOAudioProcess>) override;
        void resizeBuffers(int newSize) override;

        void work(const float *const *inputBuffer, float *const *outputBuffer,
                  const int nFrames,
                  const std::vector<int8_t> &mpcMonoInputChannelIndices,
                  const std::vector<int8_t> &mpcMonoOutputChannelIndices,
                  const std::vector<int8_t> &hostInputChannelIndices,
                  const std::vector<int8_t> &hostOutputChannelIndices);
    };
} // namespace mpc::engine::audio::server
