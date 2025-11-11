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

PushPlayCommand::PushPlayCommand(mpc::Mpc &mpc) : mpc(mpc) {}

void PushPlayCommand::execute()
{
    auto sequencer = mpc.getSequencer();
    auto hardware = mpc.getHardware();
    auto recButton = hardware->getButton(hardware::ComponentId::REC);
    auto overdubButton = hardware->getButton(hardware::ComponentId::OVERDUB);

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
        lcdgui::screengroups::isPlayAndRecordScreen(currentScreen);

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
            !mpc.getAudioMidiServices()->isBouncing())
        {
            mpc.getLayeredScreen()->openScreenById(
                ScreenId::VmpcDirectToDiskRecorderScreen);
        }
        else
        {
            if (!lcdgui::screengroups::isPlayScreen(currentScreen))
            {
                mpc.getLayeredScreen()->openScreenById(
                    ScreenId::SequencerScreen);
            }

            sequencer->setSongModeEnabled(
                mpc.getLayeredScreen()->isCurrentScreen(
                    {ScreenId::SongScreen}));
            sequencer->getTransport()->play();
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
