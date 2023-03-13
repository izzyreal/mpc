#pragma once

#include <engine/audio/core/AudioProcess.hpp>

namespace ctoot::mpc {

    class InterconnectionOutputProcess
            : public ctoot::audio::core::AudioProcess
    {

    public:
        int processAudio(ctoot::audio::core::AudioBuffer *buffer) override;

    private:
        ctoot::audio::core::AudioBuffer *sharedBuffer{nullptr};

    public:
        InterconnectionOutputProcess(ctoot::audio::core::AudioBuffer *sharedBuffer);

    };

}
