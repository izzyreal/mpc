#pragma once
#include "command/Command.h"

namespace mpc::command {

    class MainScreenCommand : public Command {
    public:
        explicit MainScreenCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
