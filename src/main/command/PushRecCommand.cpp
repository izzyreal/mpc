#include "PushRecCommand.h"
#include "Mpc.hpp"
#include "controller/ClientInputControllerBase.h"
#include "controls/Controls.hpp"
#include "hardware2/Hardware2.h"
#include "lcdgui/ScreenGroups.h"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushRecCommand::PushRecCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushRecCommand::execute()
    {
        mpc.inputController->buttonLockTracker.unlock("rec");
        mpc.inputController->buttonLockTracker.unlock("overdub");

        if (lcdgui::screengroups::isPlayOnlyScreen(mpc.getLayeredScreen()->getCurrentScreenName()))
        {
            return;
        }

        if (mpc.getSequencer()->isRecordingOrOverdubbing())
        {
            mpc.getSequencer()->setRecording(false);
            mpc.getSequencer()->setOverdubbing(false);
        }

        if (!lcdgui::screengroups::isPlayAndRecordScreen(mpc.getLayeredScreen()->getCurrentScreenName()))
        {
            mpc.getLayeredScreen()->openScreen("sequencer");
        }

        mpc.getHardware2()->getLed("rec")->setEnabled(true);
    }

}
