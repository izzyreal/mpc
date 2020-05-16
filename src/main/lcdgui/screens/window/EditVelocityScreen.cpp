#include <lcdgui/screens/window/EditVelocityScreen.hpp>

#include <lcdgui/screens/window/SeqWindowUtil.hpp>

#include <lcdgui/LayeredScreen.hpp>
#include <ui/sequencer/window/SequencerWindowGui.hpp>
#include <sequencer/Event.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sequencer;
using namespace std;

EditVelocityScreen::EditVelocityScreen(const int& layer)
	: ScreenComponent("editvelocity", layer)
{
}

void EditVelocityScreen::function(int i)
{
	BaseControls::function(i);
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();
	switch (i)
	{
	case 4:
		for (auto& event : track.lock()->getEvents())
		{
			auto ne = dynamic_pointer_cast<NoteEvent>(event.lock());

			if (ne)
			{
				if (ne->getTick() >= swGui->getTime0() && ne->getTick() <= swGui->getTime1())
				{
					if (swGui->getEditType() == 0)
					{
						ne->setVelocity(ne->getVelocity() + swGui->getValue());
					} 
					else if (swGui->getEditType() == 1)
					{
						ne->setVelocity(ne->getVelocity() - swGui->getValue());
					}
					else if (swGui->getEditType() == 2)
					{
						ne->setVelocity(ne->getVelocity() * (swGui->getValue() / 100.0));
					}
					else if (swGui->getEditType() == 3)
					{
						ne->setVelocity(swGui->getValue());
					}
				}
			}
		}
		ls.lock()->openScreen("sequencer");
		break;
	}
}

void EditVelocityScreen::turnWheel(int i)
{
	init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui();

	if (param.compare("edittype") == 0)
	{
		swGui->setEditType(swGui->getEditType() + i);
	}
	else if (param.compare("value") == 0)
	{
		swGui->setValue(swGui->getValue() + i);
	}
	
	SeqWindowUtil::checkAllTimesAndNotes(i);
}
