#pragma once

#include <engine/audio/core/AudioProcess.hpp>

namespace ctoot::mpc {

    class MpcMixerInterconnection;

    class InterconnectionInputProcess
            : public ctoot::audio::core::AudioProcess
    {

    public:
        int processAudio(ctoot::audio::core::AudioBuffer *buffer) override;

    private:
        MpcMixerInterconnection *mmi{nullptr};
        ctoot::audio::core::AudioBuffer *sharedBuffer{nullptr};

    public:
        InterconnectionInputProcess(MpcMixerInterconnection *mmi, ctoot::audio::core::AudioBuffer *sharedBuffer);
    };
}
