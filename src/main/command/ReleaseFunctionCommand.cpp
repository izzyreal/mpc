#include "ReleaseFunctionCommand.h"
#include "Mpc.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/SoundPlayer.hpp"

#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/DirectoryScreen.hpp"

using namespace mpc::command;

ReleaseFunctionCommand::ReleaseFunctionCommand(mpc::Mpc& mpc, int i)
: mpc(mpc), i(i) {}

void ReleaseFunctionCommand::execute()
{
    const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();

    switch (i) {
    case 0:
        if (currentScreenName == "step-timing-correct")
            mpc.getLayeredScreen()->openScreen<StepEditorScreen>();
        break;
    case 4:
        if (mpc.getLayeredScreen()->isCurrentScreenPopupFor<LoadScreen>())
        {
            mpc.getLayeredScreen()->openScreen<LoadScreen>();
            mpc.getAudioMidiServices()->getSoundPlayer()->enableStopEarly();
        }
        break;
    case 5: {
        auto sequencer = mpc.getSequencer();
        auto sampler = mpc.getSampler();

        if (!sequencer->isPlaying() && currentScreenName != "sequencer")
            sampler->finishBasicVoice();

        if (currentScreenName == "track-mute")
        {
            if (!sequencer->isSoloEnabled())
            {
                mpc.getLayeredScreen()->setCurrentBackground("track-mute");
            }
            sequencer->setSoloEnabled(sequencer->isSoloEnabled());
        }
        else if (mpc.getLayeredScreen()->isCurrentScreenPopupFor<DirectoryScreen>())
        {
            mpc.getLayeredScreen()->openScreen<DirectoryScreen>();
            mpc.getAudioMidiServices()->getSoundPlayer()->enableStopEarly();
        }
        break;
    }}
}
