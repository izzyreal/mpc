#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushRecCommand : public Command {
    public:
        explicit PushRecCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
