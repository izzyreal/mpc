#pragma once
#include "command/Command.h"

namespace mpc::command {

    class BankCommand : public Command {
    public:
        BankCommand(mpc::Mpc &mpc, int i);
        void execute() override;

    private:
        mpc::Mpc &mpc;
        int i;
    };

}
