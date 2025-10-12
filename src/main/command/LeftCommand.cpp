#include "LeftCommand.h"
#include "Mpc.hpp"

namespace mpc::command {

    LeftCommand::LeftCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void LeftCommand::execute() {
        if (!mpc.getLayeredScreen()->getFocusedField() || mpc.getLayeredScreen()->getFocus() == "dummy") {
            return;
        }

        mpc.getLayeredScreen()->transferLeft();
    }

} // namespace mpc::command
