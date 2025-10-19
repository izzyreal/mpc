#include "PushRecCommand.h"
#include "Mpc.hpp"
#include "controller/ClientInputControllerBase.h"
#include "hardware/ComponentId.h"
#include "hardware/Hardware.h"
#include "lcdgui/ScreenGroups.h"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushRecCommand::PushRecCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushRecCommand::execute()
    {
        mpc.inputController->buttonLockTracker.unlock(hardware::ComponentId::REC);
        mpc.inputController->buttonLockTracker.unlock(hardware::ComponentId::OVERDUB);

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
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
        }

        mpc.getHardware()->getLed(hardware::ComponentId::REC_LED)->setEnabled(true);
    }

}
