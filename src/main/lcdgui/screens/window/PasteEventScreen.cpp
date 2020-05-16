#include "PasteEventScreen.hpp"

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/StepEditorGui.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/Sequencer.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

PasteEventScreen::PasteEventScreen(const int& layer)
	: ScreenComponent("pasteevent", layer)
{
}

void PasteEventScreen::function(int i)
{
	BaseControls::function(i);
	auto seGui = mpc.getUis().lock()->getStepEditorGui();
	
	switch (i)
	{
	case 4:
		for (auto& event : seGui->getPlaceHolder()) {
			auto eventClone = track.lock()->cloneEvent(event).lock();
			eventClone->setTick(sequencer.lock()->getTickPosition());
		}
		ls.lock()->openScreen("sequencer_step");
		break;
	}
}
