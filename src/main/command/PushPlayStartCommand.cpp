#include "PushPlayStartCommand.h"
#include "Mpc.hpp"
#include "controls/Controls.hpp"
#include "hardware2/Hardware2.h"
#include "lcdgui/ScreenGroups.h"
#include "sequencer/Sequencer.hpp"

namespace mpc::command {

    PushPlayStartCommand::PushPlayStartCommand(mpc::Mpc &mpc) : mpc(mpc) {}

    void PushPlayStartCommand::execute() {
        if (mpc.getSequencer()->isPlaying()) return;

        const auto controls = mpc.getControls();
        const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();
        const bool currentScreenAllowsPlayAndRecord = lcdgui::screengroups::isPlayAndRecordScreen(currentScreenName);

        if (controls->isRecPressed()) {
            if (!currentScreenAllowsPlayAndRecord)
                mpc.getLayeredScreen()->openScreen("sequencer");
            mpc.getSequencer()->recFromStart();
        } else if (controls->isOverdubPressed()) {
            if (!currentScreenAllowsPlayAndRecord)
                mpc.getLayeredScreen()->openScreen("sequencer");
            mpc.getSequencer()->overdubFromStart();
        } else {
            if (controls->isShiftPressed()) {
                mpc.getLayeredScreen()->openScreen("vmpc-direct-to-disk-recorder");
            } else {
                if (!lcdgui::screengroups::isPlayScreen(currentScreenName))
                    mpc.getLayeredScreen()->openScreen("sequencer");

                mpc.getSequencer()->setSongModeEnabled(currentScreenName == "song");
                mpc.getSequencer()->playFromStart();
            }
        }

        mpc.getHardware2()->getLed("overdub")->setEnabled(mpc.getSequencer()->isOverdubbing());
        mpc.getHardware2()->getLed("rec")->setEnabled(mpc.getSequencer()->isRecording());
        mpc.getHardware2()->getLed("play")->setEnabled(mpc.getSequencer()->isPlaying());
    }

}
