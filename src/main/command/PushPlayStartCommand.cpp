#include "PushPlayStartCommand.h"
#include "Mpc.hpp"
#include "controller/ClientInputControllerBase.h"
#include "hardware/Hardware.h"
#include "lcdgui/ScreenGroups.h"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushPlayStartCommand::PushPlayStartCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushPlayStartCommand::execute()
    {
        if (mpc.getSequencer()->isPlaying())
        {
            return;
        }

        const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
        const bool currentScreenAllowsPlayAndRecord = lcdgui::screengroups::isPlayAndRecordScreen(currentScreenName);

        auto hardware = mpc.getHardware();

        const auto recButtonIsPressedOrLocked = hardware->getButton("rec")->isPressed() ||
                                                mpc.inputController->buttonLockTracker.isLocked("rec");

        const auto overdubButtonIsPressedOrLocked = hardware->getButton("overdub")->isPressed() ||
                                                    mpc.inputController->buttonLockTracker.isLocked("overdub");

        if (recButtonIsPressedOrLocked)
        {
            if (!currentScreenAllowsPlayAndRecord)
            {
                mpc.getLayeredScreen()->openScreen("sequencer");
            }
            
            mpc.getSequencer()->recFromStart();
        }
        else if (overdubButtonIsPressedOrLocked)
        {
            if (!currentScreenAllowsPlayAndRecord)
            {
                mpc.getLayeredScreen()->openScreen("sequencer");
            }

            mpc.getSequencer()->overdubFromStart();
        }
        else
        {
            if (hardware->getButton("shift")->isPressed())
            {
                mpc.getLayeredScreen()->openScreen("vmpc-direct-to-disk-recorder");
            }
            else
            {
                if (!lcdgui::screengroups::isPlayScreen(currentScreenName))
                {
                    mpc.getLayeredScreen()->openScreen("sequencer");
                }

                mpc.getSequencer()->setSongModeEnabled(currentScreenName == "song");
                mpc.getSequencer()->playFromStart();
            }
        }

        mpc.inputController->buttonLockTracker.unlock("rec");
        mpc.inputController->buttonLockTracker.unlock("overdub");

        mpc.getHardware()->getLed("overdub")->setEnabled(mpc.getSequencer()->isOverdubbing());
        mpc.getHardware()->getLed("rec")->setEnabled(mpc.getSequencer()->isRecording());
        mpc.getHardware()->getLed("play")->setEnabled(mpc.getSequencer()->isPlaying());
    }

}
