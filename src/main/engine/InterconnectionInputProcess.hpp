#pragma once

#include "engine/audio/core/AudioProcess.hpp"

namespace mpc::engine
{

    class MixerInterconnection;

    class InterconnectionInputProcess
        : public mpc::engine::audio::core::AudioProcess
    {

    public:
        int
        processAudio(mpc::engine::audio::core::AudioBuffer *buffer) override;

    private:
        MixerInterconnection *mmi{nullptr};
        mpc::engine::audio::core::AudioBuffer *sharedBuffer{nullptr};

    public:
        InterconnectionInputProcess(
            MixerInterconnection *mmi,
            mpc::engine::audio::core::AudioBuffer *sharedBuffer);
    };
} // namespace mpc::engine
