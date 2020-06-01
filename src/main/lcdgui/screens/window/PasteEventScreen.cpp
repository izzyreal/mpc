#include "PasteEventScreen.hpp"

#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

#include <lcdgui/screens/StepEditorScreen.hpp>
#include <lcdgui/Screens.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace std;

PasteEventScreen::PasteEventScreen(const int layerIndex)
	: ScreenComponent("pasteevent", layerIndex)
{
}

void PasteEventScreen::function(int i)
{
	BaseControls::function(i);
	
	switch (i)
	{
	case 4:
		auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(Screens::getScreenComponent("step"));
		for (auto& event : stepEditorScreen->getPlaceHolder())
		{
			auto eventClone = track.lock()->cloneEvent(event).lock();
			eventClone->setTick(sequencer.lock()->getTickPosition());
		}
		ls.lock()->openScreen("step");
		break;
	}
}
