#pragma once
#include "command/Command.h"

namespace mpc::command {

    class PlayStartCommand : public Command {
    public:
        explicit PlayStartCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
