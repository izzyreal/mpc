#include "PushTapCommand.h"
#include "Mpc.hpp"
#include "controller/ClientInputControllerBase.h"
#include "controls/Controls.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushTapCommand::PushTapCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushTapCommand::execute() {
        mpc.inputController->buttonLockTracker.unlock("tap");
        mpc.getSequencer()->tap();
    }
}

