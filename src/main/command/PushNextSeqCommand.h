#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushNextSeqCommand : public Command {
    public:
        explicit PushNextSeqCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
