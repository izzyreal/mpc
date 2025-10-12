#pragma once

#include "Command.h"

namespace mpc::command {
    class ReleaseGoToCommand : public Command {
        mpc::Mpc& mpc;
    public:
        explicit ReleaseGoToCommand(mpc::Mpc& mpc);
        void execute() override;
    };
}
