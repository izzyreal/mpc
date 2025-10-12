#pragma once
#include "command/Command.h"

namespace mpc::command {

    class EraseCommand : public Command {
    public:
        explicit EraseCommand(mpc::Mpc &mpc);
        void execute() override;

    private:
        mpc::Mpc &mpc;
    };

}
