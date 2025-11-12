#pragma once
#include "engine/audio/core/AudioProcess.hpp"

#include <vector>

namespace mpc::engine::audio::server
{
    class IOAudioProcess : public core::AudioProcess
    {
    public:
        std::vector<float> localBuffer;
        virtual std::string getName() = 0;
        virtual ~IOAudioProcess() = default;
    };
} // namespace mpc::engine::audio::server
