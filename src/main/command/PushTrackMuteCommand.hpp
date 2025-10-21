#pragma once
#include "command/Command.hpp"

namespace mpc::command {

    class PushTrackMuteCommand : public Command {
    public:
        explicit PushTrackMuteCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
