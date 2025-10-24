/**
 * An AudioServer implementation that can be used with any externally provided audio API.
 **/

#pragma once
#include <engine/audio/server/AudioServer.hpp>

#include <string>
#include <vector>
#include <memory>

namespace mpc::engine::audio::server
{
    class RealTimeAudioServer : public AudioServer
    {

    public:
        void start() override;
        void stop() override;
        bool isRunning() override;
        void close() override;
        void setClient(std::shared_ptr<AudioClient> client) override;
        IOAudioProcess *openAudioOutput(std::string name) override;
        IOAudioProcess *openAudioInput(std::string name) override;
        void closeAudioOutput(mpc::engine::audio::server::IOAudioProcess *output) override;
        void closeAudioInput(mpc::engine::audio::server::IOAudioProcess *input) override;
        void resizeBuffers(int newSize) override;

    public:
        // For compatibility with the PortAudio framework
        void work(float *inputBuffer, float *outputBuffer, int nFrames, int inputChannelCount, int outputChannelCount);

        // For compatibility with JUCE 7.0.5+
        void work(const float *const *inputBuffer,
                  float *const *outputBuffer,
                  const int nFrames,
                  const std::vector<int8_t> &mpcMonoInputChannelIndices,
                  const std::vector<int8_t> &mpcMonoOutputChannelIndices,
                  const std::vector<int8_t> &hostInputChannelIndices,
                  const std::vector<int8_t> &hostOutputChannelIndices);
    };
} // namespace mpc::engine::audio::server
