#include "UpCommand.h"
#include "Mpc.hpp"

namespace mpc::command {

    UpCommand::UpCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void UpCommand::execute() {
        if (!mpc.getLayeredScreen()->getFocusedField() || mpc.getLayeredScreen()->getFocus() == "dummy") {
            return;
        }

        mpc.getLayeredScreen()->transferUp();
    }

} // namespace mpc::command
