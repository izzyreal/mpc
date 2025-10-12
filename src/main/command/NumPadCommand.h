#pragma once
#include "command/Command.h"

namespace mpc::command {

    class NumPadCommand : public Command {
    public:
        NumPadCommand(mpc::Mpc &mpc, int i);
        void execute() override;

    private:
        mpc::Mpc &mpc;
        int i;
    };

} // namespace mpc::command
