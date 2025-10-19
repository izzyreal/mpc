#include "PushGoToCommand.h"
#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushGoToCommand::PushGoToCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushGoToCommand::execute() {

        if (!mpc.getSequencer()->getActiveSequence()->isUsed())
        {
            return;
        }

        if (mpc.getLayeredScreen()->getCurrentScreenName() == "sequencer")
        {
            mpc.getLayeredScreen()->openScreen<LocateScreen>();
        }
    }

}
