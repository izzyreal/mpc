#include "ReleaseFunctionCommand.h"
#include "Mpc.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/SoundPlayer.hpp"
#include "controls/Controls.hpp"

using namespace mpc::command;

ReleaseFunctionCommand::ReleaseFunctionCommand(mpc::Mpc& mpc, int i)
: mpc(mpc), i(i) {}

void ReleaseFunctionCommand::execute() {
    const auto controls = mpc.getControls();
    const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();

    switch (i) {
    case 0:
        if (currentScreenName == "step-timing-correct")
            mpc.getLayeredScreen()->openScreen("step-editor");
        break;
    case 2:
        controls->setF3Pressed(false);
        break;
    case 3:
        controls->setF4Pressed(false);
        break;
    case 4:
        controls->setF5Pressed(false);
        if (mpc.getLayeredScreen()->getPreviousScreenName() == "load" && currentScreenName == "popup") {
            if (const auto lastFocusedField = mpc.getLayeredScreen()->getLastFocus("load");
                lastFocusedField == "file" || lastFocusedField == "view")
            {
                mpc.getLayeredScreen()->openScreen("load");
                mpc.getAudioMidiServices()->getSoundPlayer()->enableStopEarly();
            }
        }
        break;
    case 5: {
        auto sequencer = mpc.getSequencer();
        auto sampler = mpc.getSampler();
        controls->setF6Pressed(false);

        if (!sequencer->isPlaying() && currentScreenName != "sequencer")
            sampler->finishBasicVoice();

        if (currentScreenName == "track-mute") {
            if (!sequencer->isSoloEnabled())
                mpc.getLayeredScreen()->setCurrentBackground("track-mute");
            sequencer->setSoloEnabled(sequencer->isSoloEnabled());
        } else if (mpc.getLayeredScreen()->getPreviousScreenName() == "directory" && currentScreenName == "popup") {
            mpc.getLayeredScreen()->openScreen("directory");
            mpc.getAudioMidiServices()->getSoundPlayer()->enableStopEarly();
        }
        break;
    }}
}
