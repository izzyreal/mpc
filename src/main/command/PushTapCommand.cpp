#include "PushTapCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushTapCommand::PushTapCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushTapCommand::execute() {
        const auto controls = mpc.getControls();
        if (controls->isTapPressed()) return;
        controls->setTapPressed(true);
        mpc.getSequencer()->tap();
    }

}
