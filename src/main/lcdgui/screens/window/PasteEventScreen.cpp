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

PasteEventScreen::PasteEventScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "paste-event", layerIndex)
{
}

void PasteEventScreen::function(int i)
{
	ScreenComponent::function(i);
	
	switch (i)
	{
	case 4:
		auto stepEditorScreen = dynamic_pointer_cast<StepEditorScreen>(mpc.screens->getScreenComponent("step-editor"));
		for (auto& event : stepEditorScreen->getPlaceHolder())
		{
			auto eventClone = track.lock()->cloneEvent(event).lock();
			eventClone->setTick(sequencer.lock()->getTickPosition());
		}
		openScreen("step-editor");
		break;
	}
}
