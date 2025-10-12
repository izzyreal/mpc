#pragma once
#include "Command.h"

namespace mpc::command {
    class ReleaseTapCommand : public Command {
        mpc::Mpc& mpc;
    public:
        explicit ReleaseTapCommand(mpc::Mpc& mpc);
        void execute() override;
    };
}
