#pragma once
#include "Command.hpp"

namespace mpc::command
{
    class ReleaseRecCommand : public Command
    {
        Mpc &mpc;

    public:
        explicit ReleaseRecCommand(Mpc &mpc);
        void execute() override;
    };
} // namespace mpc::command
