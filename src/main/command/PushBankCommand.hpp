#pragma once
#include "command/Command.hpp"

namespace mpc::command
{

    class PushBankCommand : public Command
    {
    public:
        PushBankCommand(mpc::Mpc &mpc, int i);
        void execute() override;

    private:
        mpc::Mpc &mpc;
        int i;
    };

} // namespace mpc::command
