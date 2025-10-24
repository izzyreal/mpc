#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushMainScreenCommand : public Command
    {
    public:
        explicit PushMainScreenCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
