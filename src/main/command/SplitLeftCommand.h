#pragma once
#include "command/Command.h"

namespace mpc::command {

    class SplitLeftCommand : public Command {
    public:
        explicit SplitLeftCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
