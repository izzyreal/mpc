#pragma once
#include "command/Command.h"

namespace mpc::command {

    class StopCommand : public Command {
    public:
        explicit StopCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
