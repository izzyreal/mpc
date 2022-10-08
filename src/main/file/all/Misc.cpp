#include <file/all/Misc.hpp>

#include <Mpc.hpp>

#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/screens/OthersScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/MidiSwScreen.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace mpc::file::all;
using namespace std;

Misc::Misc(const vector<char>& b)
{
	tapAvg = b[TAP_AVG_OFFSET];
	inReceiveMMCEnabled = b[MIDI_SYNC_IN_RECEIVE_MMC_OFFSET] > 0;
    
    for (int i = 0; i < 4; i++)
    {
        auto ctrl = b[MIDI_SWITCH_OFFSET + (i * 2)];
        auto func = b[MIDI_SWITCH_OFFSET + (i * 2) + 1];
        switches[i] = pair(ctrl, func);
    }
    
	autoStepInc = b[AUTO_STEP_INCREMENT_OFFSET] > 0;
	durationOfRecNotesTcEnabled = b[DURATION_OF_REC_NOTES_OFFSET] > 0;
	durationTcPercentage = b[DURATION_TC_PERCENTAGE_OFFSET];
	pgmChToSeqEnabled = b[MIDI_PGM_CHANGE_TO_SEQ_OFFSET] > 0;
}

Misc::Misc(mpc::Mpc& mpc)
{
	auto stepEditorScreen = mpc.screens->get<StepEditorScreen>("step-editor");
	auto othersScreen = mpc.screens->get<OthersScreen>("others");
	saveBytes = vector<char>(LENGTH);
	saveBytes[TAP_AVG_OFFSET] = (char)(othersScreen->getTapAveraging());

	auto syncScreen = mpc.screens->get<SyncScreen>("sync");

	saveBytes[MIDI_SYNC_IN_RECEIVE_MMC_OFFSET] = (char)(syncScreen->receiveMMCEnabled ? 1 : 0);
    
    auto midiSwScreen = mpc.screens->get<MidiSwScreen>("midi-sw");
    
    for (int i = 0; i < 4; i++)
    {
        auto ctrl = midiSwScreen->getSwitch(i).first;
        auto func = midiSwScreen->getSwitch(i).second;
        saveBytes[MIDI_SWITCH_OFFSET + (i * 2)] = (char) ctrl;
        saveBytes[MIDI_SWITCH_OFFSET + (i * 2) + 1] = (char) func;
    }
    
	saveBytes[AUTO_STEP_INCREMENT_OFFSET] = (char)(stepEditorScreen->isAutoStepIncrementEnabled() ? 1 : 0);
	saveBytes[DURATION_OF_REC_NOTES_OFFSET] = (char)(stepEditorScreen->isDurationTcPercentageEnabled() ? 1 : 0);
	saveBytes[DURATION_TC_PERCENTAGE_OFFSET] = (char)(stepEditorScreen->getTcValueRecordedNotes());
	saveBytes[MIDI_PGM_CHANGE_TO_SEQ_OFFSET] = 0; // Unimplemented
}

int Misc::getTapAvg()
{
    return tapAvg;
}

bool Misc::isInReceiveMMCEnabled()
{
    return inReceiveMMCEnabled;
}

bool Misc::isAutoStepIncEnabled()
{
    return autoStepInc;
}

bool Misc::isDurationOfRecNotesTc()
{
    return durationOfRecNotesTcEnabled;
}

int Misc::getDurationTcPercentage()
{
    return durationTcPercentage;
}

bool Misc::isPgmChToSeqEnabled()
{
    return pgmChToSeqEnabled;
}

vector<pair<int, int>> Misc::getSwitches()
{
    return switches;
}

vector<char>& Misc::getBytes()
{
    return saveBytes;
}
