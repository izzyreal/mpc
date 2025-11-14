#include "disk/SoundSaver.hpp"

#include "Mpc.hpp"
#include "disk/AbstractDisk.hpp"
#include "disk/MpcFile.hpp"
#include "sampler/Sound.hpp"

#include "lcdgui/screens/window/SaveAProgramScreen.hpp"
#include "lcdgui/screens/dialog2/PopupScreen.hpp"

#include "StrUtil.hpp"

#include <thread>

using namespace mpc::disk;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;

SoundSaver::SoundSaver(
    Mpc &mpc, const std::vector<std::shared_ptr<sampler::Sound>> &sounds,
    const bool isWav)
    : mpc(mpc), sounds(sounds), isWav(isWav), saveSoundsThread(std::thread(
                                                  [this]
                                                  {
                                                      saveSounds();
                                                  }))
{
}

void SoundSaver::saveSounds() const
{
    const auto ext = std::string(isWav ? ".WAV" : ".SND");
    const auto disk = mpc.getDisk();

    for (auto s : sounds)
    {
        auto fileName = StrUtil::replaceAll(s->getName(), ' ', "");

        mpc.getLayeredScreen()->openScreenById(ScreenId::PopupScreen);
        const auto popupScreen = mpc.screens->get<ScreenId::PopupScreen>();
        popupScreen->setText("Saving " + StrUtil::padRight(fileName, " ", 16) +
                             ext);

        if (disk->checkExists(fileName + ext))
        {
            const auto saveAProgramScreen =
                mpc.screens->get<ScreenId::SaveAProgramScreen>();

            if (saveAProgramScreen->replaceSameSounds)
            {
                (void)disk->getFile(fileName + ext)
                    ->del(); // possibly prepend auto success =
            }
            else
            {
                continue;
            }
        }

        if (isWav)
        {
            disk->writeWav(s, "");
        }
        else
        {
            disk->writeSnd(s, "");
        }

        std::this_thread::sleep_for(std::chrono::milliseconds(50));
    }

    mpc.getLayeredScreen()->openScreenById(ScreenId::SaveScreen);
}

SoundSaver::~SoundSaver()
{
    if (saveSoundsThread.joinable())
    {
        saveSoundsThread.join();
    }
}
