#include "PushOverdubCommand.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushOverdubCommand::PushOverdubCommand(Mpc &mpc) : mpc(mpc) {}

void PushOverdubCommand::execute()
{
    mpc.clientEventController->clientHardwareEventController->buttonLockTracker
        .unlock(hardware::ComponentId::REC);
    mpc.clientEventController->clientHardwareEventController->buttonLockTracker
        .unlock(hardware::ComponentId::OVERDUB);

    if (screengroups::isPlayOnlyScreen(
            mpc.getLayeredScreen()->getCurrentScreen()))
    {
        return;
    }

    if (mpc.getSequencer()->getTransport()->isRecordingOrOverdubbing())
    {
        mpc.getSequencer()->getTransport()->setRecording(false);
        mpc.getSequencer()->getTransport()->setOverdubbing(false);
    }

    if (!mpc.getSequencer()->getTransport()->isPlaying() &&
        !screengroups::isPlayAndRecordScreen(
            mpc.getLayeredScreen()->getCurrentScreen()))
    {
        mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
    }
}
