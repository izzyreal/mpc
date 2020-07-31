#include <file/all/Misc.hpp>

#include <Mpc.hpp>

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/screens/OthersScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace mpc::file::all;
using namespace std;

Misc::Misc(vector<char> b) 
{
	tapAvg = b[TAP_AVG_OFFSET];
	inReceiveMMCEnabled = b[MIDI_SYNC_IN_RECEIVE_MMC_OFFSET] > 0;
	autoStepInc = b[AUTO_STEP_INCREMENT_OFFSET] > 0;
	durationOfRecNotesTcEnabled = b[DURATION_OF_REC_NOTES_OFFSET] > 0;
	durationTcPercentage = b[DURATION_TC_PERCENTAGE_OFFSET];
	pgmChToSeqEnabled = b[MIDI_PGM_CHANGE_TO_SEQ_OFFSET] > 0;
}

Misc::Misc(mpc::Mpc& mpc)
{
	auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(mpc.screens->getScreenComponent("step-editor"));
	auto othersScreen = dynamic_pointer_cast<OthersScreen>(mpc.screens->getScreenComponent("others"));
	saveBytes = vector<char>(LENGTH);
	saveBytes[TAP_AVG_OFFSET] = (char)(othersScreen->getTapAveraging());

	auto syncScreen = dynamic_pointer_cast<SyncScreen>(mpc.screens->getScreenComponent("sync"));

	saveBytes[MIDI_SYNC_IN_RECEIVE_MMC_OFFSET] = (char)(syncScreen->receiveMMCEnabled ? 1 : 0);
	saveBytes[AUTO_STEP_INCREMENT_OFFSET] = (char)(stepEditorScreen->isAutoStepIncrementEnabled() ? 1 : 0);
	saveBytes[DURATION_OF_REC_NOTES_OFFSET] = (char)(stepEditorScreen->isDurationTcPercentageEnabled() ? 1 : 0);
	saveBytes[DURATION_TC_PERCENTAGE_OFFSET] = (char)(stepEditorScreen->getTcValueRecordedNotes());
	saveBytes[MIDI_PGM_CHANGE_TO_SEQ_OFFSET] = 0; // Unimplemented
}

const int Misc::LENGTH;
const int Misc::TAP_AVG_OFFSET;
const int Misc::MIDI_SYNC_IN_RECEIVE_MMC_OFFSET;
const int Misc::AUTO_STEP_INCREMENT_OFFSET;
const int Misc::DURATION_OF_REC_NOTES_OFFSET;
const int Misc::DURATION_TC_PERCENTAGE_OFFSET;
const int Misc::MIDI_PGM_CHANGE_TO_SEQ_OFFSET;

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

vector<char> Misc::getBytes()
{
    return saveBytes;
}
