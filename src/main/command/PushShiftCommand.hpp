#pragma once
#include "command/Command.hpp"

namespace mpc::command {

    class PushShiftCommand : public Command {
    public:
        explicit PushShiftCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
