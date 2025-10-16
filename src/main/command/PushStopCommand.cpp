#include "PushStopCommand.h"
#include "Mpc.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "controller/ClientInputControllerBase.h"
#include "hardware/Hardware.h"
#include "lcdgui/ScreenGroups.h"
#include "lcdgui/screens/SequencerScreen.hpp"
#include "lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp"

namespace mpc::command {

    PushStopCommand::PushStopCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushStopCommand::execute()
    {
        const auto vmpcDirectToDiskRecorderScreen = mpc.screens->get<lcdgui::screens::window::VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
        const auto ams = mpc.getAudioMidiServices();

        mpc.inputController->unlockNoteRepeat();

        if (ams->isBouncing() && (vmpcDirectToDiskRecorderScreen->getRecord() != 4 ||
                                  mpc.getHardware()->getButton("shift")->isPressed()))
        {
            ams->stopBouncingEarly();
        }

        mpc.getSequencer()->stop();

        if (!lcdgui::screengroups::isPlayScreen(mpc.getLayeredScreen()->getCurrentScreenName()))
        {
            mpc.getLayeredScreen()->openScreen("sequencer");
        }

        const auto sequencerScreen = mpc.screens->get<lcdgui::screens::SequencerScreen>("sequencer");
        sequencerScreen->hideFooterLabelAndShowFunctionKeys();

        mpc.getHardware()->getLed("overdub")->setEnabled(mpc.getSequencer()->isOverdubbing());
        mpc.getHardware()->getLed("rec")->setEnabled(mpc.getSequencer()->isRecording());
        mpc.getHardware()->getLed("play")->setEnabled(mpc.getSequencer()->isPlaying());
    }
}

