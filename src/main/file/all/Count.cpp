#include <file/all/Count.hpp>

#include <Mpc.hpp>

#include <file/all/AllParser.hpp>

#include <lcdgui/screens/window/CountMetronomeScreen.hpp>
#include <lcdgui/screens/dialog/MetronomeSoundScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::file::all;

Count::Count(const std::vector<char>& b)
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

Count::Count(mpc::Mpc& mpc)
{
	auto countMetronomeScreen = mpc.screens->get<CountMetronomeScreen>("count-metronome");
	auto metronomeSoundScreen = mpc.screens->get<MetronomeSoundScreen>("metronome-sound");

	auto lSequencer = mpc.getSequencer();

	saveBytes = std::vector<char>(AllParser::COUNT_LENGTH);
	saveBytes[ENABLED_OFFSET] = static_cast< int8_t >((lSequencer->isCountEnabled() ? 1 : 0));
	saveBytes[COUNT_IN_MODE_OFFSET] = static_cast< int8_t >(countMetronomeScreen->getCountInMode());
	saveBytes[CLICK_VOLUME_OFFSET] = static_cast< int8_t >(metronomeSoundScreen->getVolume());
	saveBytes[RATE_OFFSET] = static_cast< int8_t >(countMetronomeScreen->getRate());
	saveBytes[ENABLED_IN_PLAY_OFFSET] = static_cast< int8_t >((countMetronomeScreen->getInPlay() ? 1 : 0));
	saveBytes[ENABLED_IN_REC_OFFSET] = static_cast< int8_t >((countMetronomeScreen->getInRec() ? 1 : 0));
	saveBytes[CLICK_OUTPUT_OFFSET] = static_cast< int8_t >(metronomeSoundScreen->getOutput());
	saveBytes[WAIT_FOR_KEY_ENABLED_OFFSET] = static_cast< int8_t >((countMetronomeScreen->isWaitForKeyEnabled() ? 1 : 0));
	saveBytes[SOUND_OFFSET] = static_cast< int8_t >(metronomeSoundScreen->getSound());
	saveBytes[ACCENT_PAD_OFFSET] = static_cast< int8_t >(metronomeSoundScreen->getAccentPad());
	saveBytes[NORMAL_PAD_OFFSET] = static_cast< int8_t >(metronomeSoundScreen->getNormalPad());
	saveBytes[ACCENT_VELO_OFFSET] = static_cast< int8_t >(metronomeSoundScreen->getAccentVelo());
	saveBytes[NORMAL_VELO_OFFSET] = static_cast< int8_t >(metronomeSoundScreen->getNormalVelo());
}

bool Count::isEnabled()
{
    return enabled;
}

int Count::getCountInMode()
{
    return countInMode;
}

int Count::getClickVolume()
{
    return clickVolume;
}

int Count::getRate()
{
    return rate;
}

bool Count::isEnabledInPlay()
{
    return enabledInPlay;
}

bool Count::isEnabledInRec()
{
    return enabledInRec;
}

int Count::getClickOutput()
{
    return clickOutput;
}

bool Count::isWaitForKeyEnabled()
{
    return waitForKeyEnabled;
}

int Count::getSound()
{
    return sound;
}

int Count::getAccentPad()
{
    return accentPad;
}

int Count::getNormalPad()
{
    return normalPad;
}

int Count::getAccentVelo()
{
    return accentVelo;
}

int Count::getNormalVelo()
{
    return normalVelo;
}

std::vector<char>& Count::getBytes()
{
    return saveBytes;
}
