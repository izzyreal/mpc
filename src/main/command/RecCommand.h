#pragma once
#include "command/Command.h"

namespace mpc::command {

    class RecCommand : public Command {
    public:
        explicit RecCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
