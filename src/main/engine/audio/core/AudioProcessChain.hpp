#pragma once

#include "engine/audio/core/AudioProcess.hpp"
#include "engine/audio/core/AudioControlsChain.hpp"
#include "engine/audio/core/AudioControls.hpp"

namespace mpc::engine::audio::core
{

    class AudioProcessChain : public AudioProcess
    {

    public:
        std::shared_ptr<AudioControlsChain> controlChain;
        std::vector<std::shared_ptr<AudioProcess>> processes;

    public:
        void open() override;

    public:
        int processAudio(mpc::engine::audio::core::AudioBuffer *buffer,
                         int nFrames) override;

        void close() override;

        virtual std::string getName();

    public:
        virtual std::shared_ptr<AudioProcess>
        createProcess(std::shared_ptr<AudioControls>)
        {
            return {};
        }

    public:
        AudioProcessChain(
            const std::shared_ptr<AudioControlsChain> &controlChain);
    };

} // namespace mpc::engine::audio::core
