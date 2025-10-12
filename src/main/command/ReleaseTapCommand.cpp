#include "ReleaseTapCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;

ReleaseTapCommand::ReleaseTapCommand(mpc::Mpc& mpc) : mpc(mpc) {}

void ReleaseTapCommand::execute() {
    const auto controls = mpc.getControls();
    controls->setTapPressed(false);

    if (mpc.getSequencer()->isRecordingOrOverdubbing())
        mpc.getSequencer()->flushTrackNoteCache();

    if (!controls->isNoteRepeatLocked()) {
        const auto sequencerScreen = mpc.screens->get<lcdgui::screens::SequencerScreen>("sequencer");
        sequencerScreen->releaseTap();
    }
}
