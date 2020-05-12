#include <controls/sequencer/window/Assign16LevelsControls.hpp>

#include <Mpc.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/TopPanel.hpp>
#include <hardware/Led.hpp>
#include <ui/sequencer/SequencerGui.hpp>

using namespace mpc::controls::sequencer::window;
using namespace std;

Assign16LevelsControls::Assign16LevelsControls()
	: AbstractSequencerControls()
{
}

void Assign16LevelsControls::function(int i)
{
    super::function(i);
	switch (i)
	{
    case 4:
	{
		mpc::Mpc::instance().getHardware().lock()->getTopPanel().lock()->setSixteenLevelsEnabled(true);
		Mpc::instance().getHardware().lock()->getLed("sixteenlevels").lock()->light(true);
		ls.lock()->openScreen(ls.lock()->getPreviousScreenName());
		break;
	}
    }
}

void Assign16LevelsControls::turnWheel(int i)
{
	init();
	if (param.compare("note") == 0) {
		sequencerGui->setNote(sequencerGui->getNote() + i);
	}
	else if (param.compare("param") == 0) {
		sequencerGui->setParameter(sequencerGui->getParameter() + i);
	}
	else if (param.compare("type") == 0) {
		sequencerGui->setType(sequencerGui->getType() + i);
	}
	else if (param.compare("originalkeypad") == 0) {
		sequencerGui->setOriginalKeyPad(sequencerGui->getOriginalKeyPad() + i);
	}
}
