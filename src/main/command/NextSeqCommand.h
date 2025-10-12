#pragma once
#include "command/Command.h"

namespace mpc::command {

    class NextSeqCommand : public Command {
    public:
        explicit NextSeqCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
