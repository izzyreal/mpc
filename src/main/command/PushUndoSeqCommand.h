#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushUndoSeqCommand : public Command {
    public:
        explicit PushUndoSeqCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
