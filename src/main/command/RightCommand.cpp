#include "RightCommand.h"
#include "Mpc.hpp"

namespace mpc::command {

    RightCommand::RightCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void RightCommand::execute() {
        if (!mpc.getLayeredScreen()->getFocusedField() || mpc.getLayeredScreen()->getFocus() == "dummy") {
            return;
        }

        mpc.getLayeredScreen()->transferRight();
    }

} // namespace mpc::command
