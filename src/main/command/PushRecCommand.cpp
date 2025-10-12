#include "PushRecCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "hardware2/Hardware2.h"
#include "lcdgui/ScreenGroups.h"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushRecCommand::PushRecCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushRecCommand::execute() {
        if (lcdgui::screengroups::isPlayOnlyScreen(mpc.getLayeredScreen()->getCurrentScreenName())) return;

        auto controls = mpc.getControls();
        if (controls->isRecPressed(false)) return;

        controls->setRecPressed(true);
        controls->setRecLocked(false);

        if (mpc.getSequencer()->isRecordingOrOverdubbing()) {
            mpc.getSequencer()->setRecording(false);
            mpc.getSequencer()->setOverdubbing(false);
        }

        if (!lcdgui::screengroups::isPlayAndRecordScreen(mpc.getLayeredScreen()->getCurrentScreenName()))
            mpc.getLayeredScreen()->openScreen("sequencer");

        mpc.getHardware2()->getLed("rec")->setEnabled(true);
    }

}
