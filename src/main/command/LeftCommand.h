#pragma once
#include "command/Command.h"

namespace mpc::command {

    class LeftCommand : public Command {
    public:
        explicit LeftCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command