#pragma once
#include "command/Command.h"

namespace mpc::command {

    class UpCommand : public Command {
    public:
        explicit UpCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
