#include "FullLevelCommand.h"
#include "Mpc.hpp"
#include "hardware2/Hardware2.h"

namespace mpc::command {

    FullLevelCommand::FullLevelCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void FullLevelCommand::execute() {
        mpc.setFullLevelEnabled(!mpc.isFullLevelEnabled());
        mpc.getHardware2()->getLed("full-level")->setEnabled(mpc.isFullLevelEnabled());
    }
}

