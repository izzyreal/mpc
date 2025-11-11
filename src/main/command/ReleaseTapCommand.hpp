#pragma once
#include "Command.hpp"

namespace mpc::command
{
    class ReleaseTapCommand : public Command
    {
        Mpc &mpc;

    public:
        explicit ReleaseTapCommand(Mpc &mpc);
        void execute() override;
    };
} // namespace mpc::command
