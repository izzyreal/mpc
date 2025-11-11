#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushRightCommand : public Command
    {
    public:
        explicit PushRightCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
