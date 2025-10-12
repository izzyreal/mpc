#pragma once
#include "command/Command.h"

namespace mpc::command {

    class AfterCommand : public Command {
    public:
        explicit AfterCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
