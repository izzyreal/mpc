#include "InsertEventControls.hpp"

#include <Mpc.hpp>
#include <ui/Uis.hpp>

#include <ui/sequencer/StepEditorGui.hpp>

#include <command/InsertEvent.hpp>
#include <lcdgui/LayeredScreen.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

InsertEventControls::InsertEventControls(mpc::Mpc* mpc)
	: AbstractSequencerControls(mpc)
{
}

void InsertEventControls::function(int i)
{
	super::function(i);
	mpc::command::InsertEvent command(mpc->getUis().lock()->getStepEditorGui(), track.lock().get(), sequencer.lock().get());
	switch (i) {
	case 4:
		command.execute();
		ls.lock()->openScreen("sequencer_step");
		break;
	}
}

void InsertEventControls::turnWheel(int i)
{
	init();
	if (param.compare("eventtype") == 0) {
		seGui->setInsertEventType(seGui->getInsertEventType() + i);
	}
}
