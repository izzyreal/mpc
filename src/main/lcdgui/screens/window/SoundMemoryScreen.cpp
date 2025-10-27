#include "SoundMemoryScreen.hpp"

#include "lcdgui/PunchRect.hpp"

#include <StrUtil.hpp>

using namespace mpc::lcdgui::screens::window;

SoundMemoryScreen::SoundMemoryScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "sound-memory", layerIndex)
{
    MRECT r(23, 26, 223, 35);
    addChildT<PunchRect>("free-memory", r)->setOn(true);
}

void SoundMemoryScreen::open()
{
    displayFreeMemoryTime();
    displayIndicator();
    displayMegabytesInstalled();
}

void SoundMemoryScreen::displayFreeMemoryTime()
{
    auto total = 33374880;

    for (auto &s : sampler->getSounds())
    {
        total -= (s->getSampleData()->size() * 2);
    }

    auto time =
        StrUtil::padLeft(StrUtil::TrimDecimals(total / 176400.0, 1), " ", 6);
    findLabel("free-memory-time")->setText("Free memory(time):" + time);
}

void SoundMemoryScreen::displayIndicator()
{
    auto free = (32620.0 - sampler->getFreeSampleSpace()) / 32620.0;
    findChild<PunchRect>("free-memory")->setSize((int)floor(200.0 * free), 9);
}

void SoundMemoryScreen::displayMegabytesInstalled()
{
    findLabel("megabytes-installed")->setText("32");
}
