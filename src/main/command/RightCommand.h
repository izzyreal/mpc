#pragma once
#include "command/Command.h"

namespace mpc::command {

    class RightCommand : public Command {
    public:
        explicit RightCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
