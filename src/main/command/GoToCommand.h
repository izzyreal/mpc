#pragma once
#include "command/Command.h"

namespace mpc::command {

    class GoToCommand : public Command {
    public:
        explicit GoToCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
