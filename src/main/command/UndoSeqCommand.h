#pragma once
#include "command/Command.h"

namespace mpc::command {

    class UndoSeqCommand : public Command {
    public:
        explicit UndoSeqCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
