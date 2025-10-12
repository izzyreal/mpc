#pragma once
#include "command/Command.h"

namespace mpc::command {

    class TapCommand : public Command {
    public:
        explicit TapCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
