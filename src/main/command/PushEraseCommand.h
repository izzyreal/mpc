#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushEraseCommand : public Command {
    public:
        explicit PushEraseCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
