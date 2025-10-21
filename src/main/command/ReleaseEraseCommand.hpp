#pragma once
#include "Command.hpp"

namespace mpc::command {
    class ReleaseEraseCommand : public Command {
        mpc::Mpc& mpc;
    public:
        explicit ReleaseEraseCommand(mpc::Mpc& mpc);
        void execute() override;
    };
}
