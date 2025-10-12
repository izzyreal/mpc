#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushUpCommand : public Command {
    public:
        explicit PushUpCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
