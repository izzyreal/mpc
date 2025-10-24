#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushDownCommand : public Command
    {
    public:
        explicit PushDownCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
