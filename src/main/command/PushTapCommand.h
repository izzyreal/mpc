#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushTapCommand : public Command {
    public:
        explicit PushTapCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
