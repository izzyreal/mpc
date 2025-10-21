#pragma once
#include "Command.hpp"

namespace mpc::command {
    class ReleaseRecCommand : public Command {
        mpc::Mpc& mpc;
    public:
        explicit ReleaseRecCommand(mpc::Mpc& mpc);
        void execute() override;
    };
}
