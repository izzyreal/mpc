#pragma once

#include <cstdint>
#include <engine/audio/core/AudioBuffer.hpp>

namespace mpc::engine::audio::core {

    class AudioProcess
    {
    public:
        static const int AUDIO_OK{0};
        static const int AUDIO_DISCONNECT{1};
        static const int AUDIO_SILENCE{2};

        virtual void open() {}

        virtual int processAudio(mpc::engine::audio::core::AudioBuffer *buffer)
        { return processAudio(buffer, buffer->getSampleCount()); }

        virtual int processAudio(mpc::engine::audio::core::AudioBuffer *buffer, int)
        { return processAudio(buffer); }

        virtual void close() {}
    };
}
