#pragma once
#include "command/Command.h"

namespace mpc::command {

    class OverdubCommand : public Command {
    public:
        explicit OverdubCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
