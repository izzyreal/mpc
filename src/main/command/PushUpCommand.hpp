#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushUpCommand : public Command
    {
    public:
        explicit PushUpCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
