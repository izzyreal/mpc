#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushOverdubCommand : public Command {
    public:
        explicit PushOverdubCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
