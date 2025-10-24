#include "PushTapCommand.hpp"
#include "Mpc.hpp"
#include "controller/ClientHardwareControllerBase.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushTapCommand::PushTapCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushTapCommand::execute()
    {
        mpc.inputController->unlockNoteRepeat();
        mpc.getSequencer()->tap();
    }
}

