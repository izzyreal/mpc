#pragma once
#include "command/Command.h"

namespace mpc::command {

    class FullLevelCommand : public Command {
    public:
        explicit FullLevelCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
