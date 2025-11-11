#pragma once

#include "engine/audio/server/IOAudioProcess.hpp"

#include <string>

namespace mpc::engine::audio
{

    namespace server
    {

        class AudioServerProcess : public IOAudioProcess
        {

            std::string name;

        public:
            std::string getName() override;

            AudioServerProcess(const std::string &name);
        };

    } // namespace server
} // namespace mpc::engine::audio
