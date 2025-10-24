#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class SplitRightCommand : public Command
    {
    public:
        explicit SplitRightCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
