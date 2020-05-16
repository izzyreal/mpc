#include <lcdgui/screens/window/Assign16LevelsScreen.hpp>

#include <Mpc.hpp>
#include <lcdgui/LayeredScreen.hpp>
#include <hardware/Hardware.hpp>
#include <hardware/TopPanel.hpp>
#include <hardware/Led.hpp>

#include <ui/sequencer/window/Assign16LevelsGui.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

Assign16LevelsScreen::Assign16LevelsScreen(const int& layer)
	: ScreenComponent("assign16levels", layer)
{
}

void Assign16LevelsScreen::function(int i)
{
    BaseControls::function(i);
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

void Assign16LevelsScreen::turnWheel(int i)
{
	init();
	
	if (param.compare("note") == 0) {
		assign16LevelsGui->setNote(assign16LevelsGui->getNote() + i);
	}
	else if (param.compare("param") == 0) {
		assign16LevelsGui->setParameter(assign16LevelsGui->getParameter() + i);
	}
	else if (param.compare("type") == 0) {
		assign16LevelsGui->setType(assign16LevelsGui->getType() + i);
	}
	else if (param.compare("originalkeypad") == 0) {
		assign16LevelsGui->setOriginalKeyPad(assign16LevelsGui->getOriginalKeyPad() + i);
	}
}
