#include "PushMainScreenCommand.h"
#include "Mpc.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "hardware/Hardware.h"

namespace mpc::command {

    PushMainScreenCommand::PushMainScreenCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushMainScreenCommand::execute() {
        const auto ams = mpc.getAudioMidiServices();
        if (ams->isRecordingSound())
            ams->stopSoundRecorder();

        mpc.getLayeredScreen()->openScreen("sequencer");
        mpc.getSequencer()->setSoloEnabled(mpc.getSequencer()->isSoloEnabled());

        const auto hw = mpc.getHardware();
        hw->getLed("next-seq")->setEnabled(false);
        hw->getLed("track-mute")->setEnabled(false);
    }

}
