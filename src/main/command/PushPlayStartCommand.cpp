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

    auto hardware = mpc.getHardware();

    const auto recButtonIsPressedOrLocked =
        hardware->getButton(hardware::ComponentId::REC)->isPressed() ||
        mpc.clientEventController->clientHardwareEventController
            ->buttonLockTracker.isLocked(hardware::ComponentId::REC);

    const auto overdubButtonIsPressedOrLocked =
        hardware->getButton(hardware::ComponentId::OVERDUB)->isPressed() ||
        mpc.clientEventController->clientHardwareEventController
            ->buttonLockTracker.isLocked(hardware::ComponentId::OVERDUB);

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
        if (hardware->getButton(hardware::ComponentId::SHIFT)->isPressed())
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

            mpc.getSequencer()->setSongModeEnabled(
                mpc.getLayeredScreen()->isCurrentScreen(
                    {ScreenId::SongScreen}));
            constexpr bool fromStart = true;
            mpc.getSequencer()->getTransport()->play(fromStart);
        }
    }

    mpc.clientEventController->clientHardwareEventController->buttonLockTracker
        .unlock(hardware::ComponentId::REC);
    mpc.clientEventController->clientHardwareEventController->buttonLockTracker
        .unlock(hardware::ComponentId::OVERDUB);

    mpc.getHardware()
        ->getLed(hardware::ComponentId::OVERDUB_LED)
        ->setEnabled(mpc.getSequencer()->getTransport()->isOverdubbing());
    mpc.getHardware()
        ->getLed(hardware::ComponentId::REC_LED)
        ->setEnabled(mpc.getSequencer()->getTransport()->isRecording());
    mpc.getHardware()
        ->getLed(hardware::ComponentId::PLAY_LED)
        ->setEnabled(mpc.getSequencer()->getTransport()->isPlaying());
}
