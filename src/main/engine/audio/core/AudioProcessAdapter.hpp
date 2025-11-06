#pragma once
#include "engine/audio/core/AudioProcess.hpp"

namespace mpc::engine::audio::core
{
    class AudioProcessAdapter : public AudioProcess
    {

    protected:
        AudioProcess *process;

    public:
        int processAudio(mpc::engine::audio::core::AudioBuffer *buf,
                         int nFrames) override;

        AudioProcessAdapter(AudioProcess *process);
    };
} // namespace mpc::engine::audio::core
