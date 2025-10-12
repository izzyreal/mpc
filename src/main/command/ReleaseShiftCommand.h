#pragma once
#include "Command.h"

namespace mpc::command {
    class ReleaseShiftCommand : public Command {
        mpc::Mpc& mpc;
    public:
        explicit ReleaseShiftCommand(mpc::Mpc& mpc);
        void execute() override;
    };
}
