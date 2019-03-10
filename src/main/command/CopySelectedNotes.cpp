#include "CopySelectedNotes.hpp"

#include <ui/sequencer/StepEditorGui.hpp>
#include <sequencer/ChannelPressureEvent.hpp>
#include <sequencer/ControlChangeEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/PitchBendEvent.hpp>
#include <sequencer/PolyPressureEvent.hpp>
#include <sequencer/ProgramChangeEvent.hpp>
#include <sequencer/SystemExclusiveEvent.hpp>

using namespace mpc::command;
using namespace mpc::sequencer;
using namespace std;


CopySelectedNotes::CopySelectedNotes(mpc::ui::sequencer::StepEditorGui* seGui)
{
	this->seGui = seGui;
}

void CopySelectedNotes::execute()
{
	seGui->setSelectedEvents();
	seGui->setPlaceHolder(seGui->getSelectedEvents());
	seGui->clearSelection();
}
