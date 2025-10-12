#pragma once
#include "command/Command.h"

namespace mpc::command {

    class SplitRightCommand : public Command {
    public:
        explicit SplitRightCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
