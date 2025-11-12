#pragma once

#include "engine/audio/core/AudioProcess.hpp"

namespace mpc::engine
{

    class InterconnectionOutputProcess : public audio::core::AudioProcess
    {

    public:
        int processAudio(audio::core::AudioBuffer *buffer) override;

    private:
        audio::core::AudioBuffer *sharedBuffer{nullptr};

    public:
        InterconnectionOutputProcess(audio::core::AudioBuffer *sharedBuffer);
    };

} // namespace mpc::engine
