#include "PushEraseCommand.hpp"
#include "Mpc.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushEraseCommand::PushEraseCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushEraseCommand::execute()
    {
        if (!mpc.getSequencer()->getActiveSequence()->isUsed() ||
            mpc.getSequencer()->isPlaying())
        {
            return;
        }

        mpc.getLayeredScreen()->openScreen<EraseScreen>();
    }
}

