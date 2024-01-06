#include "MidiInput.hpp"

#include <Mpc.hpp>

#include <lcdgui/screens/window/MultiRecordingSetupScreen.hpp>
#include <lcdgui/screens/window/MidiInputScreen.hpp>
#include <lcdgui/screens/window/MidiOutputScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::file::all;

MidiInput::MidiInput(const std::vector<char>& b)
{
    softThruMode = b[SOFT_THRU_MODE_OFFSET];
	receiveCh = b[RECEIVE_CH_OFFSET];
	sustainPedalToDuration = b[SUSTAIN_PEDAL_TO_DURATION_OFFSET] > 0;
	filterEnabled = b[FILTER_ENABLED_OFFSET] > 0;
	filterType = b[FILTER_TYPE_OFFSET];
	multiRecEnabled = b[MULTI_REC_ENABLED_OFFSET] > 0;
	
	for (int i = 0; i < MULTI_REC_TRACK_DESTS_LENGTH; i++)
	{
		multiRecTrackDests[i] = b[MULTI_REC_TRACK_DESTS_OFFSET + i] - 1;
	}

	notePassEnabled = b[NOTE_PASS_ENABLED_OFFSET] > 0;
	pitchBendPassEnabled = b[PITCH_BEND_PASS_ENABLED_OFFSET] > 0;
	pgmChangePassEnabled = b[PGM_CHANGE_PASS_ENABLED_OFFSET] > 0;
	chPressurePassEnabled = b[CH_PRESSURE_PASS_ENABLED_OFFSET] > 0;
	polyPressurePassEnabled = b[POLY_PRESSURE_PASS_ENABLED_OFFSET] > 0;
	exclusivePassEnabled = b[EXCLUSIVE_PASS_ENABLED_OFFSET] > 0;
}

MidiInput::MidiInput(mpc::Mpc& mpc)
{
	saveBytes = std::vector<char>(LENGTH);

	auto midiInputScreen = mpc.screens->get<MidiInputScreen>("midi-input");

    saveBytes[SOFT_THRU_MODE_OFFSET] = mpc.screens->get<MidiOutputScreen>("midi-output")->getSoftThru();
	saveBytes[RECEIVE_CH_OFFSET] = static_cast<int8_t>(midiInputScreen->getReceiveCh());
	saveBytes[SUSTAIN_PEDAL_TO_DURATION_OFFSET] = static_cast<int8_t>(midiInputScreen->isSustainPedalToDurationEnabled() ? 1 : 0);
	saveBytes[FILTER_ENABLED_OFFSET] = static_cast<int8_t>((midiInputScreen->isMidiFilterEnabled() ? 1 : 0));
	saveBytes[FILTER_TYPE_OFFSET] = static_cast<int8_t>(midiInputScreen->getType());
	saveBytes[MULTI_REC_ENABLED_OFFSET] = static_cast<int8_t>(mpc.getSequencer()->isRecordingModeMulti() ? 1 : 0);
	
	auto screen = mpc.screens->get<MultiRecordingSetupScreen>("multi-recording-setup");

	for (int i = 0; i < MULTI_REC_TRACK_DESTS_LENGTH; i++)
	{
		saveBytes[MULTI_REC_TRACK_DESTS_OFFSET + i] = static_cast<int8_t>(screen->getMrsLines()[i]->getTrack() + 1);
	}

	saveBytes[NOTE_PASS_ENABLED_OFFSET] = static_cast<int8_t>(notePassEnabled ? 1 : 0);
	saveBytes[PITCH_BEND_PASS_ENABLED_OFFSET] = static_cast<int8_t>(pitchBendPassEnabled ? 1 : 0);
	saveBytes[PGM_CHANGE_PASS_ENABLED_OFFSET] = static_cast<int8_t>(pgmChangePassEnabled ? 1 : 0);
	saveBytes[CH_PRESSURE_PASS_ENABLED_OFFSET] = static_cast<int8_t>(chPressurePassEnabled ? 1 : 0);
	saveBytes[POLY_PRESSURE_PASS_ENABLED_OFFSET] = static_cast<int8_t>(polyPressurePassEnabled ? 1 : 0);
	saveBytes[EXCLUSIVE_PASS_ENABLED_OFFSET] = static_cast<int8_t>(exclusivePassEnabled ? 1 : 0);
}

int MidiInput::getSoftThruMode()
{
    return softThruMode;
}

int MidiInput::getReceiveCh()
{
    return receiveCh;
}

bool MidiInput::isSustainPedalToDurationEnabled()
{
    return sustainPedalToDuration;
}

bool MidiInput::isFilterEnabled()
{
    return filterEnabled;
}

int MidiInput::getFilterType()
{
    return filterType;
}

bool MidiInput::isMultiRecEnabled()
{
    return multiRecEnabled;
}

std::vector<int> MidiInput::getMultiRecTrackDests()
{
    return multiRecTrackDests;
}

bool MidiInput::isNotePassEnabled()
{
    return notePassEnabled;
}

bool MidiInput::isPitchBendPassEnabled()
{
    return pitchBendPassEnabled;
}

bool MidiInput::isPgmChangePassEnabled()
{
    return pgmChangePassEnabled;
}

bool MidiInput::isChPressurePassEnabled()
{
    return chPressurePassEnabled;
}

bool MidiInput::isPolyPressurePassEnabled()
{
    return polyPressurePassEnabled;
}

bool MidiInput::isExclusivePassEnabled()
{
    return exclusivePassEnabled;
}

std::vector<char>& MidiInput::getBytes()
{
    return saveBytes;
}
