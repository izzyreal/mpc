#include "PushFullLevelCommand.h"
#include "Mpc.hpp"
#include "hardware/Hardware.h"

namespace mpc::command {

    PushFullLevelCommand::PushFullLevelCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushFullLevelCommand::execute() {
        mpc.setFullLevelEnabled(!mpc.isFullLevelEnabled());
        mpc.getHardware()->getLed(hardware::ComponentId::FULL_LEVEL_OR_CASE_SWITCH_LED)->setEnabled(mpc.isFullLevelEnabled());
    }
}

