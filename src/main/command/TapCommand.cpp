#include "TapCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    TapCommand::TapCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void TapCommand::execute() {
        const auto controls = mpc.getControls();
        if (controls->isTapPressed()) return;
        controls->setTapPressed(true);
        mpc.getSequencer()->tap();
    }

}
