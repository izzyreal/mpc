#pragma once
#include "command/Command.h"

namespace mpc::command {

    class FunctionCommand : public Command {
    public:
        FunctionCommand(mpc::Mpc &mpc, int i);
        void execute() override;

    private:
        mpc::Mpc &mpc;
        int i;
    };

} // namespace mpc::command
