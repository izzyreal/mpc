#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushOverdubCommand : public Command
    {
    public:
        explicit PushOverdubCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
