#include "PushLeftCommand.h"
#include "Mpc.hpp"

namespace mpc::command {

    PushLeftCommand::PushLeftCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushLeftCommand::execute() {
        if (!mpc.getLayeredScreen()->getFocusedField() || mpc.getLayeredScreen()->getFocus() == "dummy") {
            return;
        }

        mpc.getLayeredScreen()->transferLeft();
    }

} // namespace mpc::command
