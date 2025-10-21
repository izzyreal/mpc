#pragma once
#include "command/Command.hpp"

namespace mpc::command {

    class PushAfterCommand : public Command {
    public:
        explicit PushAfterCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
