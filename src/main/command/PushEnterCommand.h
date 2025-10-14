#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushEnterCommand : public Command {
    public:
        explicit PushEnterCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
