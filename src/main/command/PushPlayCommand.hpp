#pragma once
#include "command/Command.hpp"

namespace mpc::command {

    class PushPlayCommand : public Command {
    public:
        explicit PushPlayCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}

