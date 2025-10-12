#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PressEnterCommand : public Command {
    public:
        explicit PressEnterCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
