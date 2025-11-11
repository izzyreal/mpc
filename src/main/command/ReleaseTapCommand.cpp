#include "ReleaseTapCommand.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

ReleaseTapCommand::ReleaseTapCommand(Mpc &mpc) : mpc(mpc) {}

void ReleaseTapCommand::execute()
{
    if (mpc.getSequencer()->getTransport()->isRecordingOrOverdubbing())
    {
        mpc.getSequencer()->flushTrackNoteCache();
    }

    if (!mpc.clientEventController->clientHardwareEventController
             ->isNoteRepeatLocked())
    {
        const auto sequencerScreen =
            mpc.screens->get<ScreenId::SequencerScreen>();
        sequencerScreen->hideFooterLabelAndShowFunctionKeys();
    }
}
