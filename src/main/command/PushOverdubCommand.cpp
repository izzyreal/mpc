#include "PushOverdubCommand.h"
#include "Mpc.hpp"
#include "controller/ClientInputControllerBase.h"
#include "hardware/Hardware.h"
#include "lcdgui/ScreenGroups.h"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushOverdubCommand::PushOverdubCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushOverdubCommand::execute()
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

        if (!mpc.getSequencer()->isPlaying() && !lcdgui::screengroups::isPlayAndRecordScreen(mpc.getLayeredScreen()->getCurrentScreenName()))
        {
            mpc.getLayeredScreen()->openScreen("sequencer");
        }

        mpc.getHardware()->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(true);
    }

}
