#pragma once
#include "engine/audio/core/AudioProcess.hpp"

namespace mpc::engine::audio::core
{
    class AudioProcessAdapter : public AudioProcess
    {

    protected:
        AudioProcess *process;

    public:
        int processAudio(AudioBuffer *buf, int nFrames) override;

        AudioProcessAdapter(AudioProcess *process);
    };
} // namespace mpc::engine::audio::core
