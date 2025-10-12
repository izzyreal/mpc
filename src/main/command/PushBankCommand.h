#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushBankCommand : public Command {
    public:
        PushBankCommand(mpc::Mpc &mpc, int i);
        void execute() override;

    private:
        mpc::Mpc &mpc;
        int i;
    };

}
