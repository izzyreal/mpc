#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushPlayStartCommand : public Command {
    public:
        explicit PushPlayStartCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
