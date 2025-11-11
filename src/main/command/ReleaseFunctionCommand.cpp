#include "ReleaseFunctionCommand.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "audiomidi/SoundPlayer.hpp"

#include "lcdgui/screens/LoadScreen.hpp"
#include "lcdgui/screens/window/DirectoryScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

ReleaseFunctionCommand::ReleaseFunctionCommand(Mpc &mpc, int i)
    : mpc(mpc), i(i)
{
}

void ReleaseFunctionCommand::execute()
{
    const auto ls = mpc.getLayeredScreen();
    switch (i)
    {
        case 0:
            if (ls->isCurrentScreen({ScreenId::StepTcScreen}))
            {
                mpc.getLayeredScreen()->openScreenById(
                    ScreenId::StepEditorScreen);
            }
            break;
        case 2:
            if (ls->isCurrentScreen({ScreenId::LoadASoundScreen}))
            {
                mpc.getSampler()->finishBasicVoice();
            }
            break;
        case 4:
            if (mpc.getLayeredScreen()->isCurrentScreenPopupFor(
                    ScreenId::LoadScreen))
            {
                mpc.getLayeredScreen()->openScreenById(ScreenId::LoadScreen);
                mpc.getEngineHost()->getSoundPlayer()->enableStopEarly();
            }
            break;
        case 5:
        {
            auto sequencer = mpc.getSequencer();
            auto sampler = mpc.getSampler();

            if (!sequencer->getTransport()->isPlaying() &&
                !ls->isCurrentScreen({ScreenId::SequencerScreen}))
            {
                sampler->finishBasicVoice();
            }

            if (ls->isCurrentScreen({ScreenId::TrMuteScreen}))
            {
                if (!sequencer->isSoloEnabled())
                {
                    mpc.getLayeredScreen()->setCurrentBackground("track-mute");
                }

                sequencer->setSoloEnabled(sequencer->isSoloEnabled());
            }
            else if (mpc.getLayeredScreen()->isCurrentScreenPopupFor(
                         ScreenId::DirectoryScreen))
            {
                mpc.getLayeredScreen()->openScreenById(
                    ScreenId::DirectoryScreen);
                mpc.getEngineHost()->getSoundPlayer()->enableStopEarly();
            }
            break;
        }
    }
}
