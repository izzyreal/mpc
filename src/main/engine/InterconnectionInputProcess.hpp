#pragma once

#include "engine/audio/core/AudioProcess.hpp"

namespace mpc::engine
{

    class MixerInterconnection;

    class InterconnectionInputProcess : public audio::core::AudioProcess
    {

    public:
        int processAudio(audio::core::AudioBuffer *buffer) override;

    private:
        MixerInterconnection *mmi{nullptr};
        audio::core::AudioBuffer *sharedBuffer{nullptr};

    public:
        InterconnectionInputProcess(MixerInterconnection *mmi,
                                    audio::core::AudioBuffer *sharedBuffer);
    };
} // namespace mpc::engine
