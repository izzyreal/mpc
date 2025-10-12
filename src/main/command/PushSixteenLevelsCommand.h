#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PushSixteenLevelsCommand : public Command {
    public:
        explicit PushSixteenLevelsCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
