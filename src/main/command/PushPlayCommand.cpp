#include "PushPlayCommand.h"
#include "Mpc.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "controller/ClientInputControllerBase.h"
#include "hardware/ComponentId.h"
#include "hardware/Hardware.h"
#include "lcdgui/ScreenGroups.h"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushPlayCommand::PushPlayCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushPlayCommand::execute()
    {
        auto sequencer = mpc.getSequencer();
        auto hardware = mpc.getHardware();
        auto recButton = hardware->getButton(hardware::ComponentId::REC);
        auto overdubButton = hardware->getButton(hardware::ComponentId::OVERDUB);

        if (sequencer->isPlaying())
        {
            if (recButton->isPressed() && !sequencer->isOverdubbing())
            {
                sequencer->setOverdubbing(false);
                sequencer->setRecording(true);
            }
            else if (overdubButton->isPressed() && !sequencer->isRecording())
            {
                sequencer->setOverdubbing(true);
                sequencer->setRecording(false);
            }

            return;
        }

        const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
        const bool isPlayAndRecordScreen = lcdgui::screengroups::isPlayAndRecordScreen(currentScreenName);

        const auto recButtonIsPressedOrLocked = hardware->getButton(hardware::ComponentId::REC)->isPressed() ||
                                                mpc.inputController->buttonLockTracker.isLocked(hardware::ComponentId::REC);

        const auto overdubButtonIsPressedOrLocked = hardware->getButton(hardware::ComponentId::OVERDUB)->isPressed() ||
                                                mpc.inputController->buttonLockTracker.isLocked(hardware::ComponentId::OVERDUB);

        if (recButtonIsPressedOrLocked)
        {
            if (!isPlayAndRecordScreen)
            {
                mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            }

            sequencer->rec();
        }
        else if (overdubButtonIsPressedOrLocked)
        {
            if (!isPlayAndRecordScreen)
            {
                mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            }

            sequencer->overdub();
        }
        else {
            if (hardware->getButton(hardware::ComponentId::SHIFT)->isPressed() && !mpc.getAudioMidiServices()->isBouncing())
            {
                mpc.getLayeredScreen()->openScreen<VmpcDirectToDiskRecorderScreen>();
            }
            else
            {
                if (!lcdgui::screengroups::isPlayScreen(currentScreenName))
                {
                    mpc.getLayeredScreen()->openScreen<SequencerScreen>();
                }

                sequencer->setSongModeEnabled(currentScreenName == "song");
                sequencer->play();
            }
        }

        mpc.inputController->buttonLockTracker.unlock(hardware::ComponentId::REC);
        mpc.inputController->buttonLockTracker.unlock(hardware::ComponentId::OVERDUB);

        mpc.getHardware()->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(mpc.getSequencer()->isOverdubbing());
        mpc.getHardware()->getLed(hardware::ComponentId::REC_LED)->setEnabled(mpc.getSequencer()->isRecording());
        mpc.getHardware()->getLed(hardware::ComponentId::PLAY_LED)->setEnabled(mpc.getSequencer()->isPlaying());
    }
}

