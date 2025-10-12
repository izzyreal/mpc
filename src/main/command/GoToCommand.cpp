#include "GoToCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    GoToCommand::GoToCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void GoToCommand::execute() {
        if (!mpc.getSequencer()->getActiveSequence()->isUsed()) return;

        const auto controls = mpc.getControls();
        controls->setGoToPressed(true);

        if (mpc.getLayeredScreen()->getCurrentScreenName() == "sequencer")
            mpc.getLayeredScreen()->openScreen("locate");
    }

}
