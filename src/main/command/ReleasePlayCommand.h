#pragma once
#include "Command.h"

namespace mpc::command {
    class ReleasePlayCommand : public Command {
        mpc::Mpc& mpc;
    public:
        explicit ReleasePlayCommand(mpc::Mpc& mpc);
        void execute() override;
    };
}
