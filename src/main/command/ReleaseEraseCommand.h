#pragma once
#include "Command.h"

namespace mpc::command {
    class ReleaseEraseCommand : public Command {
        mpc::Mpc& mpc;
    public:
        explicit ReleaseEraseCommand(mpc::Mpc& mpc);
        void execute() override;
    };
}
