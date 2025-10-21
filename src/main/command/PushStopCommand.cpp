#include "PushStopCommand.hpp"
#include "Mpc.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "controller/ClientInputControllerBase.hpp"
#include "hardware/Hardware.hpp"
#include "lcdgui/ScreenGroups.hpp"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"

namespace mpc::command {

    PushStopCommand::PushStopCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushStopCommand::execute()
    {
        const auto vmpcDirectToDiskRecorderScreen = mpc.screens->get<lcdgui::screens::window::VmpcDirectToDiskRecorderScreen>();
        const auto ams = mpc.getAudioMidiServices();

        mpc.inputController->unlockNoteRepeat();

        if (ams->isBouncing() && (vmpcDirectToDiskRecorderScreen->getRecord() != 4 ||
                                  mpc.getHardware()->getButton(hardware::ComponentId::SHIFT)->isPressed()))
        {
            ams->stopBouncingEarly();
        }

        mpc.getSequencer()->stop();

        if (!lcdgui::screengroups::isPlayScreen(mpc.getLayeredScreen()->getCurrentScreen()))
        {
            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
        }

        const auto sequencerScreen = mpc.screens->get<lcdgui::screens::SequencerScreen>();
        sequencerScreen->hideFooterLabelAndShowFunctionKeys();

        mpc.getHardware()->getLed(hardware::ComponentId::OVERDUB_LED)->setEnabled(mpc.getSequencer()->isOverdubbing());
        mpc.getHardware()->getLed(hardware::ComponentId::REC_LED)->setEnabled(mpc.getSequencer()->isRecording());
        mpc.getHardware()->getLed(hardware::ComponentId::PLAY_LED)->setEnabled(mpc.getSequencer()->isPlaying());
    }
}

