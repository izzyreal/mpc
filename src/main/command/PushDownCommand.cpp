#include "PushDownCommand.h"
#include "Mpc.hpp"

namespace mpc::command {

    PushDownCommand::PushDownCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushDownCommand::execute() {
        if (!mpc.getLayeredScreen()->getFocusedField() || mpc.getLayeredScreen()->getFocusedFieldName() == "dummy") {
            return;
        }

        mpc.getLayeredScreen()->transferDown();
    }

} // namespace mpc::command
