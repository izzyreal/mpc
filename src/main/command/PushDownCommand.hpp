#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushDownCommand : public Command
    {
    public:
        explicit PushDownCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
