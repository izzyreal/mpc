#include "PushPlayStartCommand.hpp"
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

PushPlayStartCommand::PushPlayStartCommand(Mpc &mpc) : mpc(mpc) {}

void PushPlayStartCommand::execute()
{
    if (mpc.getSequencer()->getTransport()->isPlaying())
    {
        return;
    }

    const auto currentScreen = mpc.getLayeredScreen()->getCurrentScreen();
    const bool currentScreenAllowsPlayAndRecord =
        screengroups::isPlayAndRecordScreen(currentScreen);

    const auto recButtonIsPressedOrLocked =
        mpc.clientEventController->clientHardwareEventController
            ->isRecLockedOrPressed();

    const auto overdubButtonIsPressedOrLocked =
        mpc.clientEventController->clientHardwareEventController
            ->isOverdubLockedOrPressed();

    if (recButtonIsPressedOrLocked)
    {
        if (!currentScreenAllowsPlayAndRecord)
        {
            mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
        }

        mpc.getSequencer()->getTransport()->recFromStart();
    }
    else if (overdubButtonIsPressedOrLocked)
    {
        if (!currentScreenAllowsPlayAndRecord)
        {
            mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
        }

        mpc.getSequencer()->getTransport()->overdubFromStart();
    }
    else
    {
        if (mpc.getHardware()
                ->getButton(hardware::ComponentId::SHIFT)
                ->isPressed())
        {
            mpc.getLayeredScreen()->openScreenById(
                ScreenId::VmpcDirectToDiskRecorderScreen);
        }
        else
        {
            if (!screengroups::isPlayScreen(currentScreen))
            {
                mpc.getLayeredScreen()->openScreenById(
                    ScreenId::SequencerScreen);
            }

            constexpr bool fromStart = true;
            mpc.getSequencer()->getTransport()->play(fromStart);
        }
    }

    mpc.clientEventController->clientHardwareEventController->unlockRec();
    mpc.clientEventController->clientHardwareEventController->unlockOverdub();
}
