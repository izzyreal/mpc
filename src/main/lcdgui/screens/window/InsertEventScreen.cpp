#include "InsertEventScreen.hpp"

#include <Mpc.hpp>
#include <ui/Uis.hpp>

#include <ui/sequencer/StepEditorGui.hpp>

#include <command/InsertEvent.hpp>
#include <lcdgui/LayeredScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

InsertEventScreen::InsertEventScreen(const int& layer)
	: ScreenComponent("insertevent", layer)
{
}

void InsertEventScreen::function(int i)
{
	BaseControls::function(i);
	mpc::command::InsertEvent command(Mpc::instance().getUis().lock()->getStepEditorGui(), track.lock().get(), sequencer.lock().get());
	switch (i) {
	case 4:
		command.execute();
		ls.lock()->openScreen("sequencer_step");
		break;
	}
}

void InsertEventScreen::turnWheel(int i)
{
	init();
	
	auto seGui = mpc.getUis().lock()->getStepEditorGui();

	if (param.compare("eventtype") == 0)
	{
		seGui->setInsertEventType(seGui->getInsertEventType() + i);
	}
}
