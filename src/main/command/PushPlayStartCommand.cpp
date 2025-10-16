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

        const auto recButtonIsPressedOrLocked = hardware->getButton(hardware::ComponentId::REC)->isPressed() ||
                                                mpc.inputController->buttonLockTracker.isLocked("rec");

        const auto overdubButtonIsPressedOrLocked = hardware->getButton(hardware::ComponentId::OVERDUB)->isPressed() ||
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
            if (hardware->getButton(hardware::ComponentId::SHIFT)->isPressed())
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

        mpc.getHardware()->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(mpc.getSequencer()->isOverdubbing());
        mpc.getHardware()->getLed(hardware::ComponentId::REC_LED)->setEnabled(mpc.getSequencer()->isRecording());
        mpc.getHardware()->getLed(hardware::ComponentId::PLAY_LED)->setEnabled(mpc.getSequencer()->isPlaying());
    }

}
