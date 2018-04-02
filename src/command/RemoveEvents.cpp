#include "RemoveEvents.hpp"

#include <ui/sequencer/StepEditorGui.hpp>
#include <sequencer/EmptyEvent.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>

using namespace mpc::command;
using namespace mpc::sequencer;
using namespace std;

RemoveEvents::RemoveEvents(mpc::ui::sequencer::StepEditorGui* seqGui, Track* track) 
{
	this->seqGui = seqGui;
	this->track = track;
}


void RemoveEvents::execute()
{
    auto firstEventIndex = seqGui->getSelectionStartIndex();
    auto lastEventIndex = seqGui->getSelectionEndIndex();
    if (firstEventIndex > lastEventIndex) {
        firstEventIndex = seqGui->getSelectionEndIndex();
        lastEventIndex = seqGui->getSelectionStartIndex();
    }
	int eventCounter = 0;
	vector<weak_ptr<Event>> tempList;
	for (auto& e : seqGui->getEventsAtCurrentTick()) {
		tempList.push_back(e);
	}
	for (auto& event : tempList) {
		if (eventCounter >= firstEventIndex && eventCounter <= lastEventIndex) {
			if (!dynamic_pointer_cast<mpc::sequencer::EmptyEvent>(event.lock())) {
				track->removeEvent(event);
			}
		}
		eventCounter++;
	}
    seqGui->clearSelection();
    seqGui->setyOffset(0);
}
