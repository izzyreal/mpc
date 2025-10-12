#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushTrackMuteCommand : public Command {
    public:
        explicit PushTrackMuteCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
