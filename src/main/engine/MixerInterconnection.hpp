#pragma once

#include "engine/audio/server/AudioServer.hpp"

#include <memory>

namespace mpc::engine
{

    class MixerInterconnection
    {

        std::shared_ptr<audio::core::AudioProcess> inputProcess;
        std::shared_ptr<audio::core::AudioProcess> outputProcess;

        bool leftEnabled{true};
        bool rightEnabled{true};

    public:
        std::shared_ptr<audio::core::AudioProcess> getInputProcess();

        std::shared_ptr<audio::core::AudioProcess> getOutputProcess();

        void setLeftEnabled(bool b);

        void setRightEnabled(bool b);

        bool isLeftEnabled() const;

        bool isRightEnabled() const;

        MixerInterconnection(const std::string &name,
                             audio::server::AudioServer *server);
    };
} // namespace mpc::engine
