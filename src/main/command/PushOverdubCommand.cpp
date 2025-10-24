#include "PushOverdubCommand.hpp"
#include "Mpc.hpp"
#include "controller/ClientHardwareControllerBase.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushOverdubCommand::PushOverdubCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushOverdubCommand::execute()
    {
        mpc.inputController->buttonLockTracker.unlock(hardware::ComponentId::REC);
        mpc.inputController->buttonLockTracker.unlock(hardware::ComponentId::OVERDUB);

        if (lcdgui::screengroups::isPlayOnlyScreen(mpc.getLayeredScreen()->getCurrentScreen()))
        {
            return;
        }

        if (mpc.getSequencer()->isRecordingOrOverdubbing())
        {
            mpc.getSequencer()->setRecording(false);
            mpc.getSequencer()->setOverdubbing(false);
        }

        if (!mpc.getSequencer()->isPlaying() && !lcdgui::screengroups::isPlayAndRecordScreen(mpc.getLayeredScreen()->getCurrentScreen()))
        {
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
        }

        mpc.getHardware()->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(true);
    }

}
