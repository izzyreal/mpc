#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushRightCommand : public Command
    {
    public:
        explicit PushRightCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
