#include "PushRecCommand.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "hardware/ComponentId.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushRecCommand::PushRecCommand(Mpc &mpc) : mpc(mpc) {}

void PushRecCommand::execute()
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

    const auto transport = mpc.getSequencer()->getTransport();

    if (transport->isRecordingOrOverdubbing())
    {
        transport->setRecording(false);
        transport->setOverdubbing(false);
    }

    if (!screengroups::isPlayAndRecordScreen(
            mpc.getLayeredScreen()->getCurrentScreen()))
    {
        mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
    }

    if (mpc.clientEventController->isRecMainWithoutPlaying())
    {
        transport->setPositionTicksToReturnToWhenReleasingRec(
            transport->getTickPosition());
    }
}
