#pragma once

#include <engine/audio/core/AudioProcess.hpp>

namespace ctoot::mpc {

    class MixerInterconnection;

    class InterconnectionInputProcess
            : public ctoot::audio::core::AudioProcess
    {

    public:
        int processAudio(ctoot::audio::core::AudioBuffer *buffer) override;

    private:
        MixerInterconnection *mmi{nullptr};
        ctoot::audio::core::AudioBuffer *sharedBuffer{nullptr};

    public:
        InterconnectionInputProcess(MixerInterconnection *mmi, ctoot::audio::core::AudioBuffer *sharedBuffer);
    };
}
