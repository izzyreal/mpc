#pragma once
#include "Command.h"

namespace mpc::command {
    class ReleaseRecCommand : public Command {
        mpc::Mpc& mpc;
    public:
        explicit ReleaseRecCommand(mpc::Mpc& mpc);
        void execute() override;
    };
}
