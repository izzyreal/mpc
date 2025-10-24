#pragma once

#include "Command.hpp"

namespace mpc::command
{
    class ReleaseFunctionCommand : public Command
    {
        mpc::Mpc &mpc;
        int i;

    public:
        ReleaseFunctionCommand(mpc::Mpc &mpc, int i);
        void execute() override;
    };
} // namespace mpc::command
