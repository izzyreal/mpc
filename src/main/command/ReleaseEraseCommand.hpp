#pragma once
#include "Command.hpp"

namespace mpc::command
{
    class ReleaseEraseCommand : public Command
    {
        Mpc &mpc;

    public:
        explicit ReleaseEraseCommand(Mpc &mpc);
        void execute() override;
    };
} // namespace mpc::command
