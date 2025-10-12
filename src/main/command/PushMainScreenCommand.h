#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushMainScreenCommand : public Command {
    public:
        explicit PushMainScreenCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
