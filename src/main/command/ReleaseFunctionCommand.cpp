#include "ReleaseFunctionCommand.hpp"
#include "sequencer/Transport.hpp"
#include "Mpc.hpp"
#include "engine/EngineHost.hpp"
#include "audiomidi/SoundPlayer.hpp"
#include "engine/SequencerPlaybackEngine.hpp"

#include "lcdgui/screens/window/DirectoryScreen.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::lcdgui;

ReleaseFunctionCommand::ReleaseFunctionCommand(Mpc &mpc, const int i)
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
                mpc.getEngineHost()->finishPreviewSoundPlayerVoice();
            }
            break;
        case 4:
        {
            if (mpc.getEngineHost()->getSoundPlayer()->isPlaying() ||
                ls->isCurrentScreenPopupFor(ScreenId::LoadScreen))
            {
                const std::function stopSoundPlayer =
                    [ls, engineHost = mpc.getEngineHost()]
                {
                    if (ls->isCurrentScreenPopupFor(ScreenId::LoadScreen))
                    {
                        engineHost->getSoundPlayer()->enableStopEarly();
                        ls->postToUiThread(
                            [ls]
                            {
                                ls->openScreenById(ScreenId::LoadScreen);
                            });
                    }
                };

                mpc.getEngineHost()
                    ->getSequencerPlaybackEngine()
                    ->enqueueEventAfterNFrames(stopSoundPlayer, 0);
            }
            break;
        }
        case 5:
        {
            const auto sequencer = mpc.getSequencer();
            auto sampler = mpc.getSampler();

            if (!sequencer->getTransport()->isPlaying() &&
                !ls->isCurrentScreen({ScreenId::SequencerScreen}))
            {
                mpc.getEngineHost()->finishPreviewSoundPlayerVoice();
            }

            if (ls->isCurrentScreen({ScreenId::TrMuteScreen}))
            {
                if (!sequencer->isSoloEnabled())
                {
                    mpc.getLayeredScreen()->setCurrentBackground("track-mute");
                }

                sequencer->setSoloEnabled(sequencer->isSoloEnabled());
            }
            else
            {
                if (mpc.getEngineHost()->getSoundPlayer()->isPlaying() ||
                    ls->isCurrentScreenPopupFor(ScreenId::DirectoryScreen))
                {
                    const std::function stopSoundPlayer =
                        [ls, engineHost = mpc.getEngineHost()]
                    {
                        if (ls->isCurrentScreenPopupFor(
                                ScreenId::DirectoryScreen))
                        {
                            engineHost->getSoundPlayer()->enableStopEarly();
                            ls->postToUiThread(
                                [ls]
                                {
                                    ls->openScreenById(
                                        ScreenId::DirectoryScreen);
                                });
                        }
                    };

                    mpc.getEngineHost()
                        ->getSequencerPlaybackEngine()
                        ->enqueueEventAfterNFrames(stopSoundPlayer, 0);
                }
            }
            break;
        }
        default:;
    }
}
