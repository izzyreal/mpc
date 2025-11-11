#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class SplitRightCommand : public Command
    {
    public:
        explicit SplitRightCommand(Mpc &mpc);
        void execute() override;

    private:
        Mpc &mpc;
    };

} // namespace mpc::command
