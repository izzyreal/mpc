#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushGoToCommand : public Command
    {
    public:
        explicit PushGoToCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
