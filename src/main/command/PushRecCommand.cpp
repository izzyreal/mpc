#include "PushRecCommand.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "hardware/ComponentId.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushRecCommand::PushRecCommand(mpc::Mpc &mpc) : mpc(mpc) {}

void PushRecCommand::execute()
{
    mpc.clientEventController->clientHardwareEventController->buttonLockTracker
        .unlock(hardware::ComponentId::REC);
    mpc.clientEventController->clientHardwareEventController->buttonLockTracker
        .unlock(hardware::ComponentId::OVERDUB);

    if (lcdgui::screengroups::isPlayOnlyScreen(
            mpc.getLayeredScreen()->getCurrentScreen()))
    {
        return;
    }

    if (mpc.getSequencer()->getTransport()->isRecordingOrOverdubbing())
    {
        mpc.getSequencer()->getTransport()->setRecording(false);
        mpc.getSequencer()->getTransport()->setOverdubbing(false);
    }

    if (!lcdgui::screengroups::isPlayAndRecordScreen(
            mpc.getLayeredScreen()->getCurrentScreen()))
    {
        mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
    }

    mpc.getHardware()->getLed(hardware::ComponentId::REC_LED)->setEnabled(true);
}
