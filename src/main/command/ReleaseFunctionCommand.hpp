#pragma once

#include "Command.hpp"

namespace mpc::command
{
    class ReleaseFunctionCommand : public Command
    {
        Mpc &mpc;
        int i;

    public:
        ReleaseFunctionCommand(Mpc &mpc, int i);
        void execute() override;
    };
} // namespace mpc::command
