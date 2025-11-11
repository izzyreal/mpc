#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushShiftCommand : public Command
    {
    public:
        explicit PushShiftCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
