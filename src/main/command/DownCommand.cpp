#include "DownCommand.h"
#include "Mpc.hpp"

namespace mpc::command {

    DownCommand::DownCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void DownCommand::execute() {
        if (!mpc.getLayeredScreen()->getFocusedField() || mpc.getLayeredScreen()->getFocus() == "dummy") {
            return;
        }

        mpc.getLayeredScreen()->transferDown();
    }

} // namespace mpc::command
