#include "StepTcScreen.hpp"

#include <lcdgui/Screens.hpp>
#include <lcdgui/screens/window/TimingCorrectScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace std;

StepTcScreen::StepTcScreen(const int& layer)
	: ScreenComponent("step_tc", layer)
{
}

void StepTcScreen::open()
{
	displayTcValue();
}

void StepTcScreen::displayTcValue()
{
	auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timingcorrect"));
	auto noteValue = timingCorrectScreen->getNoteValue();
	findField("tcvalue").lock()->setText(timingCorrectNames[noteValue]);
}
