#pragma once
#include "command/Command.h"

namespace mpc::command {

    class ShiftCommand : public Command {
    public:
        explicit ShiftCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
