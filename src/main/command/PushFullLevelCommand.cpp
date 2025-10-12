#include "PushFullLevelCommand.h"
#include "Mpc.hpp"
#include "hardware2/Hardware2.h"

namespace mpc::command {

    PushFullLevelCommand::PushFullLevelCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushFullLevelCommand::execute() {
        mpc.setFullLevelEnabled(!mpc.isFullLevelEnabled());
        mpc.getHardware2()->getLed("full-level")->setEnabled(mpc.isFullLevelEnabled());
    }
}

