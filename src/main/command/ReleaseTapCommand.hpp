#pragma once
#include "Command.hpp"

namespace mpc::command
{
    class ReleaseTapCommand : public Command
    {
        mpc::Mpc &mpc;

    public:
        explicit ReleaseTapCommand(mpc::Mpc &mpc);
        void execute() override;
    };
} // namespace mpc::command
