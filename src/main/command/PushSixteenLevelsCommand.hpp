#pragma once
#include "command/Command.hpp"

namespace mpc::command {

    class PushSixteenLevelsCommand : public Command {
    public:
        explicit PushSixteenLevelsCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
