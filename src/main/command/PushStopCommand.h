#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushStopCommand : public Command {
    public:
        explicit PushStopCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
