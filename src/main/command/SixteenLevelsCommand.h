#pragma once
#include "command/Command.h"

namespace mpc::command {

    class SixteenLevelsCommand : public Command {
    public:
        explicit SixteenLevelsCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
