#include "PushPlayStartCommand.hpp"
#include "Mpc.hpp"
#include "controller/ClientInputControllerBase.hpp"
#include "hardware/ComponentId.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushPlayStartCommand::PushPlayStartCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushPlayStartCommand::execute()
    {
        if (mpc.getSequencer()->isPlaying())
        {
            return;
        }

        const auto currentScreen = mpc.getLayeredScreen()->getCurrentScreen();
        const bool currentScreenAllowsPlayAndRecord = lcdgui::screengroups::isPlayAndRecordScreen(currentScreen);

        auto hardware = mpc.getHardware();

        const auto recButtonIsPressedOrLocked = hardware->getButton(hardware::ComponentId::REC)->isPressed() ||
                                                mpc.inputController->buttonLockTracker.isLocked(hardware::ComponentId::REC);

        const auto overdubButtonIsPressedOrLocked = hardware->getButton(hardware::ComponentId::OVERDUB)->isPressed() ||
                                                mpc.inputController->buttonLockTracker.isLocked(hardware::ComponentId::OVERDUB);

        if (recButtonIsPressedOrLocked)
        {
            if (!currentScreenAllowsPlayAndRecord)
            {
                mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            }
            
            mpc.getSequencer()->recFromStart();
        }
        else if (overdubButtonIsPressedOrLocked)
        {
            if (!currentScreenAllowsPlayAndRecord)
            {
                mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            }

            mpc.getSequencer()->overdubFromStart();
        }
        else
        {
            if (hardware->getButton(hardware::ComponentId::SHIFT)->isPressed())
            {
                mpc.getLayeredScreen()->openScreen<VmpcDirectToDiskRecorderScreen>();
            }
            else
            {
                if (!lcdgui::screengroups::isPlayScreen(currentScreen))
                {
                    mpc.getLayeredScreen()->openScreen<SequencerScreen>();
                }

                mpc.getSequencer()->setSongModeEnabled(mpc.getLayeredScreen()->isCurrentScreen<SongScreen>());
                mpc.getSequencer()->playFromStart();
            }
        }

        mpc.inputController->buttonLockTracker.unlock(hardware::ComponentId::REC);
        mpc.inputController->buttonLockTracker.unlock(hardware::ComponentId::OVERDUB);

        mpc.getHardware()->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(mpc.getSequencer()->isOverdubbing());
        mpc.getHardware()->getLed(hardware::ComponentId::REC_LED)->setEnabled(mpc.getSequencer()->isRecording());
        mpc.getHardware()->getLed(hardware::ComponentId::PLAY_LED)->setEnabled(mpc.getSequencer()->isPlaying());
    }

}
