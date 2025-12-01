#include "PushPlayCommand.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "hardware/ComponentId.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

PushPlayCommand::PushPlayCommand(Mpc &mpc) : mpc(mpc) {}

void PushPlayCommand::execute()
{
    const auto sequencer = mpc.getSequencer();
    const auto hardware = mpc.getHardware();
    const auto recButton = hardware->getButton(hardware::ComponentId::REC);
    const auto overdubButton =
        hardware->getButton(hardware::ComponentId::OVERDUB);

    if (sequencer->getTransport()->isPlaying())
    {
        if (recButton->isPressed() &&
            !sequencer->getTransport()->isOverdubbing())
        {
            sequencer->getTransport()->setOverdubbing(false);
            sequencer->getTransport()->setRecording(true);
        }
        else if (overdubButton->isPressed() &&
                 !sequencer->getTransport()->isRecording())
        {
            sequencer->getTransport()->setOverdubbing(true);
            sequencer->getTransport()->setRecording(false);
        }

        return;
    }

    const auto currentScreen = mpc.getLayeredScreen()->getCurrentScreen();
    const bool isPlayAndRecordScreen =
        screengroups::isPlayAndRecordScreen(currentScreen);

    const auto recButtonIsPressedOrLocked =
        mpc.clientEventController->clientHardwareEventController
            ->isRecLockedOrPressed();

    const auto overdubButtonIsPressedOrLocked =
        mpc.clientEventController->clientHardwareEventController
            ->isOverdubLockedOrPressed();

    if (recButtonIsPressedOrLocked)
    {
        if (!isPlayAndRecordScreen)
        {
            mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
        }

        sequencer->getTransport()->rec();
    }
    else if (overdubButtonIsPressedOrLocked)
    {
        if (!isPlayAndRecordScreen)
        {
            mpc.getLayeredScreen()->openScreenById(ScreenId::SequencerScreen);
        }

        sequencer->getTransport()->overdub();
    }
    else
    {
        if (hardware->getButton(hardware::ComponentId::SHIFT)->isPressed() &&
            !mpc.getEngineHost()->isBouncing())
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

            sequencer->getTransport()->play();
        }
    }

    mpc.clientEventController->clientHardwareEventController->buttonLockTracker
        .unlock(hardware::ComponentId::REC);
    mpc.clientEventController->clientHardwareEventController->buttonLockTracker
        .unlock(hardware::ComponentId::OVERDUB);
}
