#include "PasteEventControls.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

PasteEventControls::PasteEventControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void PasteEventControls::function(int i)
{
	super::function(i);
	switch (i) {
	case 4:
		for (auto& event : seGui->getPlaceHolder()) {
			auto eventClone = track.lock()->cloneEvent(event).lock();
			eventClone->setTick(sequencer.lock()->getTickPosition());
		}
		ls.lock()->openScreen("sequencer_step");
		break;
	}
}
