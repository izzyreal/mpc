#include "PushTapCommand.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command
{

    PushTapCommand::PushTapCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushTapCommand::execute()
    {
        mpc.clientEventController->clientHardwareEventController
            ->unlockNoteRepeat();
        mpc.getSequencer()->tap();
    }
} // namespace mpc::command
