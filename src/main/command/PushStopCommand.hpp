#pragma once
#include "command/Command.hpp"

namespace mpc::command {

    class PushStopCommand : public Command {
    public:
        explicit PushStopCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
