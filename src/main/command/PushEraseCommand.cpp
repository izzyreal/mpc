#include "PushEraseCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushEraseCommand::PushEraseCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushEraseCommand::execute() {
        const auto controls = mpc.getControls();
        controls->setErasePressed(true);

        if (!mpc.getSequencer()->getActiveSequence()->isUsed()) return;
        if (!mpc.getSequencer()->isRecordingOrOverdubbing())
            mpc.getLayeredScreen()->openScreen("erase");
    }

}
