#include "InsertEventScreen.hpp"

#include <Mpc.hpp>
#include <ui/Uis.hpp>

#include <command/InsertEvent.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

InsertEventScreen::InsertEventScreen(const int& layer)
	: ScreenComponent("insertevent", layer)
{
}

void InsertEventScreen::open()
{
	displayInsertEventType();
}

void InsertEventScreen::displayInsertEventType()
{
	findField("eventtype").lock()->setText(eventTypeNames[insertEventType]);
}

void InsertEventScreen::function(int i)
{
	BaseControls::function(i);
	
	switch (i)
	{
	case 4:
	{
		mpc::command::InsertEvent(insertEventType, track.lock().get(), sequencer.lock().get()).execute();
		ls.lock()->openScreen("sequencer_step");
		break;
	}
	}
}

void InsertEventScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("eventtype") == 0)
	{
		setInsertEventType(insertEventType + i);
	}
}

void InsertEventScreen::setInsertEventType(int i)
{
	if (i < 0 || i > 7)
	{
		return;
	}
	insertEventType = i;
	displayInsertEventType();
}
