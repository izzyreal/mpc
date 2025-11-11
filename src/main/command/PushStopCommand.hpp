#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushStopCommand : public Command
    {
    public:
        explicit PushStopCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
