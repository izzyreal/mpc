#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushDownCommand : public Command {
    public:
        explicit PushDownCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
