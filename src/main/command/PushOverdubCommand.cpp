#include "PushOverdubCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "hardware2/Hardware2.h"
#include "lcdgui/ScreenGroups.h"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushOverdubCommand::PushOverdubCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushOverdubCommand::execute() {
        if (lcdgui::screengroups::isPlayOnlyScreen(mpc.getLayeredScreen()->getCurrentScreenName())) return;

        auto controls = mpc.getControls();
        if (controls->isOverdubPressed(false)) return;

        controls->setOverdubPressed(true);
        controls->setOverdubLocked(false);

        if (mpc.getSequencer()->isRecordingOrOverdubbing()) {
            mpc.getSequencer()->setRecording(false);
            mpc.getSequencer()->setOverdubbing(false);
        }

        if (!lcdgui::screengroups::isPlayAndRecordScreen(mpc.getLayeredScreen()->getCurrentScreenName()))
            mpc.getLayeredScreen()->openScreen("sequencer");

        mpc.getHardware2()->getLed("overdub")->setEnabled(true);
    }

}
