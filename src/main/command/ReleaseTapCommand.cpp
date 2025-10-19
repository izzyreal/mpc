#include "ReleaseTapCommand.h"
#include "Mpc.hpp"
#include "controller/ClientInputControllerBase.h"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;

ReleaseTapCommand::ReleaseTapCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleaseTapCommand::execute()
{
    if (mpc.getSequencer()->isRecordingOrOverdubbing())
    {
        mpc.getSequencer()->flushTrackNoteCache();
    }

    if (!mpc.inputController->isNoteRepeatLocked())
    {
        const auto sequencerScreen = mpc.screens->get<lcdgui::screens::SequencerScreen>();
        sequencerScreen->hideFooterLabelAndShowFunctionKeys();
    }
}

