#include "file/all/Count.hpp"
#include "sequencer/Transport.hpp"

#include "Mpc.hpp"

#include "file/all/AllParser.hpp"

#include "lcdgui/screens/window/CountMetronomeScreen.hpp"
#include "lcdgui/screens/dialog/MetronomeSoundScreen.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Transport.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::file::all;

Count::Count(const std::vector<char> &b)
{
    enabled = b[ENABLED_OFFSET] > 0;
    countInMode = b[COUNT_IN_MODE_OFFSET];
    clickVolume = b[CLICK_VOLUME_OFFSET];
    rate = b[RATE_OFFSET];
    enabledInPlay = b[ENABLED_IN_PLAY_OFFSET] > 0;
    enabledInRec = b[ENABLED_IN_REC_OFFSET] > 0;
    clickOutput = b[CLICK_OUTPUT_OFFSET];
    waitForKeyEnabled = b[WAIT_FOR_KEY_ENABLED_OFFSET] > 0;
    sound = b[SOUND_OFFSET];
    accentPad = b[ACCENT_PAD_OFFSET];
    normalPad = b[NORMAL_PAD_OFFSET];
    accentVelo = b[ACCENT_VELO_OFFSET];
    normalVelo = b[NORMAL_VELO_OFFSET];
}

Count::Count(mpc::Mpc &mpc)
{
    auto countMetronomeScreen =
        mpc.screens->get<ScreenId::CountMetronomeScreen>();
    auto metronomeSoundScreen =
        mpc.screens->get<ScreenId::MetronomeSoundScreen>();

    auto lSequencer = mpc.getSequencer();

    saveBytes = std::vector<char>(AllParser::COUNT_LENGTH);
    saveBytes[ENABLED_OFFSET] = static_cast<int8_t>(
        (lSequencer->getTransport()->isCountEnabled() ? 1 : 0));
    saveBytes[COUNT_IN_MODE_OFFSET] =
        static_cast<int8_t>(countMetronomeScreen->getCountInMode());
    saveBytes[CLICK_VOLUME_OFFSET] =
        static_cast<int8_t>(metronomeSoundScreen->getVolume());
    saveBytes[RATE_OFFSET] =
        static_cast<int8_t>(countMetronomeScreen->getRate());
    saveBytes[ENABLED_IN_PLAY_OFFSET] =
        static_cast<int8_t>((countMetronomeScreen->getInPlay() ? 1 : 0));
    saveBytes[ENABLED_IN_REC_OFFSET] =
        static_cast<int8_t>((countMetronomeScreen->getInRec() ? 1 : 0));
    saveBytes[CLICK_OUTPUT_OFFSET] =
        static_cast<int8_t>(metronomeSoundScreen->getOutput());
    saveBytes[WAIT_FOR_KEY_ENABLED_OFFSET] = static_cast<int8_t>(
        (countMetronomeScreen->isWaitForKeyEnabled() ? 1 : 0));
    saveBytes[SOUND_OFFSET] =
        static_cast<int8_t>(metronomeSoundScreen->getSound());
    saveBytes[ACCENT_PAD_OFFSET] =
        static_cast<int8_t>(metronomeSoundScreen->getAccentPad());
    saveBytes[NORMAL_PAD_OFFSET] =
        static_cast<int8_t>(metronomeSoundScreen->getNormalPad());
    saveBytes[ACCENT_VELO_OFFSET] =
        static_cast<int8_t>(metronomeSoundScreen->getAccentVelo());
    saveBytes[NORMAL_VELO_OFFSET] =
        static_cast<int8_t>(metronomeSoundScreen->getNormalVelo());
}

bool Count::isEnabled() const
{
    return enabled;
}

int Count::getCountInMode() const
{
    return countInMode;
}

int Count::getClickVolume() const
{
    return clickVolume;
}

int Count::getRate() const
{
    return rate;
}

bool Count::isEnabledInPlay() const
{
    return enabledInPlay;
}

bool Count::isEnabledInRec() const
{
    return enabledInRec;
}

int Count::getClickOutput() const
{
    return clickOutput;
}

bool Count::isWaitForKeyEnabled() const
{
    return waitForKeyEnabled;
}

int Count::getSound() const
{
    return sound;
}

int Count::getAccentPad() const
{
    return accentPad;
}

int Count::getNormalPad() const
{
    return normalPad;
}

int Count::getAccentVelo() const
{
    return accentVelo;
}

int Count::getNormalVelo() const
{
    return normalVelo;
}

std::vector<char> &Count::getBytes()
{
    return saveBytes;
}
