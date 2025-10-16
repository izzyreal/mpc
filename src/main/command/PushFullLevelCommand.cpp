#include "PushFullLevelCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"

namespace mpc::command {

    PushFullLevelCommand::PushFullLevelCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushFullLevelCommand::execute() {
        mpc.setFullLevelEnabled(!mpc.isFullLevelEnabled());
        mpc.getHardware()->getLed("full-level")->setEnabled(mpc.isFullLevelEnabled());
    }
}

