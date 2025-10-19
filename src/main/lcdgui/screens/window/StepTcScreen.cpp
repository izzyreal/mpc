#include "StepTcScreen.hpp"

#include <lcdgui/screens/window/TimingCorrectScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui;

StepTcScreen::StepTcScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "step-timing-correct", layerIndex)
{
}

void StepTcScreen::open()
{
	displayTcValue();
}

void StepTcScreen::turnWheel(int i)
{
	auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>();
	timingCorrectScreen->setNoteValue(timingCorrectScreen->getNoteValue() + i);
	displayTcValue();
}

void StepTcScreen::displayTcValue()
{
	auto timingCorrectScreen = mpc.screens->get<TimingCorrectScreen>();
	auto noteValue = timingCorrectScreen->getNoteValue();
	findField("tcvalue")->setText(timingCorrectNames[noteValue]);
}
