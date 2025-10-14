#include "PushPlayCommand.h"
#include "Mpc.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "controller/ClientInputControllerBase.h"
#include "hardware2/Hardware2.h"
#include "lcdgui/ScreenGroups.h"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushPlayCommand::PushPlayCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushPlayCommand::execute()
    {
        auto sequencer = mpc.getSequencer();
        auto hardware = mpc.getHardware2();
        auto recButton = hardware->getButton("rec");
        auto overdubButton = hardware->getButton("overdub");

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

        const auto recButtonIsPressedOrLocked = hardware->getButton("rec")->isPressed() ||
                                                mpc.inputController->buttonLockTracker.isLocked("rec");

        const auto overdubButtonIsPressedOrLocked = hardware->getButton("overdub")->isPressed() ||
                                                    mpc.inputController->buttonLockTracker.isLocked("overdub");

        if (recButtonIsPressedOrLocked)
        {
            if (!isPlayAndRecordScreen)
            {
                mpc.getLayeredScreen()->openScreen("sequencer");
            }

            sequencer->rec();
        }
        else if (overdubButtonIsPressedOrLocked)
        {
            if (!isPlayAndRecordScreen)
            {
                mpc.getLayeredScreen()->openScreen("sequencer");
            }

            sequencer->overdub();
        }
        else {
            if (hardware->getButton("shift")->isPressed() && !mpc.getAudioMidiServices()->isBouncing())
            {
                mpc.getLayeredScreen()->openScreen("vmpc-direct-to-disk-recorder");
            }
            else
            {
                if (!lcdgui::screengroups::isPlayScreen(currentScreenName))
                {
                    mpc.getLayeredScreen()->openScreen("sequencer");
                }

                sequencer->setSongModeEnabled(currentScreenName == "song");
                sequencer->play();
            }
        }

        mpc.inputController->buttonLockTracker.unlock("rec");
        mpc.inputController->buttonLockTracker.unlock("overdub");

        mpc.getHardware2()->getLed("overdub")->setEnabled(mpc.getSequencer()->isOverdubbing());
        mpc.getHardware2()->getLed("rec")->setEnabled(mpc.getSequencer()->isRecording());
        mpc.getHardware2()->getLed("play")->setEnabled(mpc.getSequencer()->isPlaying());
    }
}

