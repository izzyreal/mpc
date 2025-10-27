#include "ReleaseTapCommand.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;

ReleaseTapCommand::ReleaseTapCommand(mpc::Mpc &mpc) : mpc(mpc) {}

void ReleaseTapCommand::execute()
{
    if (mpc.getSequencer()->isRecordingOrOverdubbing())
    {
        mpc.getSequencer()->flushTrackNoteCache();
    }

    if (!mpc.clientEventController->clientHardwareEventController
             ->isNoteRepeatLocked())
    {
        const auto sequencerScreen =
            mpc.screens->get<lcdgui::screens::SequencerScreen>();
        sequencerScreen->hideFooterLabelAndShowFunctionKeys();
    }
}
