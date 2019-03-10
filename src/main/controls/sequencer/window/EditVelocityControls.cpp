#include <controls/sequencer/window/EditVelocityControls.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>

using namespace mpc::controls::sequencer::window;
using namespace mpc::sequencer;
using namespace std;

EditVelocityControls::EditVelocityControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void EditVelocityControls::function(int i)
{
	super::function(i);
	switch (i) {
	case 4:
		for (auto& event : track.lock()->getEvents()) {
			auto ne = dynamic_pointer_cast<NoteEvent>(event.lock());
			if (ne) {
				if (ne->getTick() >= swGui->getTime0() && ne->getTick() <= swGui->getTime1()) {
					if (swGui->getEditType() == 0) {
						ne->setVelocity(ne->getVelocity() + swGui->getValue());
					}
					if (swGui->getEditType() == 1) {
						ne->setVelocity(ne->getVelocity() - swGui->getValue());
					}
					if (swGui->getEditType() == 2) {
						ne->setVelocity(ne->getVelocity() * (swGui->getValue() / 100.0));
					}
					if (swGui->getEditType() == 3) {
						ne->setVelocity(swGui->getValue());
					}
				}
			}
		}
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void EditVelocityControls::turnWheel(int i)
{
	init();
	if (param.compare("edittype") == 0) {
		swGui->setEditType(swGui->getEditType() + i);
	}
	else if (param.compare("value") == 0) {
		swGui->setValue(swGui->getValue() + i);
	}
	checkAllTimesAndNotes(i);
}
