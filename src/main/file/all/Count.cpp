#include <file/all/Count.hpp>

#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <file/all/AllParser.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::file::all;
using namespace std;

Count::Count(vector<char> b) 
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
	accentVelo = b[ACCENT_VELO_OFFSET];
	normalVelo = b[NORMAL_VELO_OFFSET];
}

Count::Count() 
{
	auto swgui = Mpc::instance().getUis().lock()->getSequencerWindowGui();
	auto lSequencer = Mpc::instance().getSequencer().lock();
	saveBytes = vector<char>(AllParser::COUNT_LENGTH);
	saveBytes[ENABLED_OFFSET] = static_cast< int8_t >((lSequencer->isCountEnabled() ? 1 : 0));
	saveBytes[COUNT_IN_MODE_OFFSET] = static_cast< int8_t >(swgui->getCountInMode());
	saveBytes[CLICK_VOLUME_OFFSET] = static_cast< int8_t >(swgui->getClickVolume());
	saveBytes[RATE_OFFSET] = static_cast< int8_t >(swgui->getRate());
	saveBytes[ENABLED_IN_PLAY_OFFSET] = static_cast< int8_t >((swgui->getInPlay() ? 1 : 0));
	saveBytes[ENABLED_IN_REC_OFFSET] = static_cast< int8_t >((swgui->getInRec() ? 1 : 0));
	saveBytes[CLICK_OUTPUT_OFFSET] = static_cast< int8_t >(swgui->getClickOutput());
	saveBytes[WAIT_FOR_KEY_ENABLED_OFFSET] = static_cast< int8_t >((swgui->isWaitForKeyEnabled() ? 1 : 0));
	saveBytes[SOUND_OFFSET] = static_cast< int8_t >(swgui->getMetronomeSound());
	saveBytes[ACCENT_VELO_OFFSET] = static_cast< int8_t >(swgui->getAccentVelo());
	saveBytes[NORMAL_VELO_OFFSET] = static_cast< int8_t >(swgui->getNormalVelo());
}

const int Count::ENABLED_OFFSET;
const int Count::COUNT_IN_MODE_OFFSET;
const int Count::CLICK_VOLUME_OFFSET;
const int Count::RATE_OFFSET;
const int Count::ENABLED_IN_PLAY_OFFSET;
const int Count::ENABLED_IN_REC_OFFSET;
const int Count::CLICK_OUTPUT_OFFSET;
const int Count::WAIT_FOR_KEY_ENABLED_OFFSET;
const int Count::SOUND_OFFSET;
const int Count::ACCENT_VELO_OFFSET;
const int Count::NORMAL_VELO_OFFSET;

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

int Count::getAccentVelo()
{
    return accentVelo;
}

int Count::getNormalVelo()
{
    return normalVelo;
}

vector<char> Count::getBytes()
{
    return saveBytes;
}
