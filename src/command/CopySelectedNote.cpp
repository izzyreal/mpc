#include "CopySelectedNote.hpp"

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
using namespace std;

CopySelectedNote::CopySelectedNote(string focus, mpc::ui::sequencer::StepEditorGui* seGui) {
	this->focus = focus;
	this->seGui = seGui;
}

void CopySelectedNote::execute()
{
    auto eventNumber = stoi(focus.substr(1, 1));
	vector<weak_ptr<mpc::sequencer::Event>> tempList{ seGui->getVisibleEvents()[eventNumber] };
    seGui->setPlaceHolder(tempList);
}
