#pragma once

#include "engine/audio/core/AudioProcess.hpp"

namespace mpc::engine {

    class InterconnectionOutputProcess
            : public mpc::engine::audio::core::AudioProcess
    {

    public:
        int processAudio(mpc::engine::audio::core::AudioBuffer *buffer) override;

    private:
        mpc::engine::audio::core::AudioBuffer *sharedBuffer{nullptr};

    public:
        InterconnectionOutputProcess(mpc::engine::audio::core::AudioBuffer *sharedBuffer);

    };

}
