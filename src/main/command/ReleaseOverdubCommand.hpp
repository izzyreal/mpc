#pragma once
#include "Command.hpp"

namespace mpc::command
{
    class ReleaseOverdubCommand : public Command
    {
        mpc::Mpc &mpc;

    public:
        explicit ReleaseOverdubCommand(mpc::Mpc &mpc);
        void execute() override;
    };
} // namespace mpc::command
