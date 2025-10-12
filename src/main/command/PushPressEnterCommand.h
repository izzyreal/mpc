#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushPressEnterCommand : public Command {
    public:
        explicit PushPressEnterCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

} // namespace mpc::command
