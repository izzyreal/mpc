#pragma once

#include <engine/audio/core/AudioProcess.hpp>
#include <engine/audio/core/AudioControlsChain.hpp>
#include <engine/audio/core/AudioControls.hpp>

#include <cstdint>

namespace ctoot::audio::core {

    class AudioProcessChain
            : public AudioProcess
    {

    public:
        std::shared_ptr<AudioControlsChain> controlChain;
        std::vector<std::shared_ptr<AudioProcess>> processes;

    public:
        void open() override;

    public:
        int processAudio(ctoot::audio::core::AudioBuffer *buffer, int nFrames) override;

        void close() override;

        virtual std::string getName();

    public:
        virtual std::shared_ptr<AudioProcess> createProcess(std::shared_ptr<AudioControls> controls) {
            return {};
        }

    public:
        AudioProcessChain(std::shared_ptr<AudioControlsChain> controlChain);

    };

}
