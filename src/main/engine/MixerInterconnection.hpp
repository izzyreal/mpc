#pragma once

#include "engine/audio/server/AudioServer.hpp"

#include <memory>

namespace mpc::engine
{

    class MixerInterconnection
    {

    private:
        std::shared_ptr<mpc::engine::audio::core::AudioProcess> inputProcess;
        std::shared_ptr<mpc::engine::audio::core::AudioProcess> outputProcess;

    private:
        bool leftEnabled{true};
        bool rightEnabled{true};

    public:
        std::shared_ptr<mpc::engine::audio::core::AudioProcess>
        getInputProcess();

        std::shared_ptr<mpc::engine::audio::core::AudioProcess>
        getOutputProcess();

    public:
        void setLeftEnabled(bool b);

        void setRightEnabled(bool b);

        bool isLeftEnabled() const;

        bool isRightEnabled() const;

    public:
        MixerInterconnection(const std::string &name,
                             mpc::engine::audio::server::AudioServer *server);
    };
} // namespace mpc::engine
