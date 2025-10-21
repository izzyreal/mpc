#pragma once
#include "command/Command.hpp"

namespace mpc::command {

    class PushNumPadCommand : public Command {
    public:
        PushNumPadCommand(mpc::Mpc &mpc, int i);
        void execute() override;

    private:
        mpc::Mpc &mpc;
        int i;
    };

} // namespace mpc::command
