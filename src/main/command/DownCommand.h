#pragma once
#include "command/Command.h"

namespace mpc::command {

    class DownCommand : public Command {
    public:
        explicit DownCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
