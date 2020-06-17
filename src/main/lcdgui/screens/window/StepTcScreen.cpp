#include "StepTcScreen.hpp"

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;
using namespace std;

StepTcScreen::StepTcScreen(const int layerIndex)
	: ScreenComponent("step-timing-correct", layerIndex)
{
}

void StepTcScreen::open()
{
	displayTcValue();
}

void StepTcScreen::turnWheel(int i)
{
	auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timing-correct"));
	timingCorrectScreen->setNoteValue(timingCorrectScreen->getNoteValue() + i);
	displayTcValue();
}

void StepTcScreen::displayTcValue()
{
	auto timingCorrectScreen = dynamic_pointer_cast<TimingCorrectScreen>(Screens::getScreenComponent("timing-correct"));
	auto noteValue = timingCorrectScreen->getNoteValue();
	findField("tcvalue").lock()->setText(timingCorrectNames[noteValue]);
}
