#pragma once
#include "Command.hpp"

namespace mpc::command
{
    class ReleaseOverdubCommand : public Command
    {
        Mpc &mpc;

    public:
        explicit ReleaseOverdubCommand(Mpc &mpc);
        void execute() override;
    };
} // namespace mpc::command
