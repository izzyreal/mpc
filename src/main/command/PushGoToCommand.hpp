#pragma once
#include "command/Command.hpp"

namespace mpc::command {

    class PushGoToCommand : public Command {
    public:
        explicit PushGoToCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
