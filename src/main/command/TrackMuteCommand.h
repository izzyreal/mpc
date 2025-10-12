#pragma once
#include "command/Command.h"

namespace mpc::command {

    class TrackMuteCommand : public Command {
    public:
        explicit TrackMuteCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
