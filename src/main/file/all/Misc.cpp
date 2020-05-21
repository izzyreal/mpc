#include <file/all/Misc.hpp>

#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <ui/midisync/MidiSyncGui.hpp>

#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/StepEditorScreen.hpp>

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

Misc::Misc()
{
	auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(Screens::getScreenComponent("sequencer_step"));
	saveBytes = vector<char>(LENGTH);
	saveBytes[TAP_AVG_OFFSET] = (char)(Mpc::instance().getUis().lock()->getSequencerWindowGui()->getTapAvg());
	saveBytes[MIDI_SYNC_IN_RECEIVE_MMC_OFFSET] = (char)(Mpc::instance().getUis().lock()->getMidiSyncGui()->isReceiveMMCEnabled() ? 1 : 0);
	saveBytes[AUTO_STEP_INCREMENT_OFFSET] = (char)(stepEditorScreen->isAutoStepIncrementEnabled() ? 1 : 0);
	saveBytes[DURATION_OF_REC_NOTES_OFFSET] = (char)(stepEditorScreen->isDurationTcPercentageEnabled() ? 1 : 0);
	saveBytes[DURATION_TC_PERCENTAGE_OFFSET] = (char)(stepEditorScreen->getTcValueRecordedNotes());
	saveBytes[MIDI_PGM_CHANGE_TO_SEQ_OFFSET] = (char)(Mpc::instance().getUis().lock()->getSequencerWindowGui()->isPgmChangeToSeqEnabled() ? 1 : 0);
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
