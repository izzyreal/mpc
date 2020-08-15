#include "SelectDrumScreen.hpp"

#include <sampler/Program.hpp>
#include <controls/BaseSamplerControls.hpp>

#include <lcdgui/screens/DrumScreen.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace std;

SelectDrumScreen::SelectDrumScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "select-drum", layerIndex)
{
	baseControls = make_shared<BaseSamplerControls>(mpc);
}

void SelectDrumScreen::open()
{
	auto drumScreen = dynamic_pointer_cast<DrumScreen>(mpc.screens->getScreenComponent("drum"));
	ls.lock()->setFunctionKeysArrangement(drumScreen->drum);
}

void SelectDrumScreen::function(int i)
{
	init();

	if (i > 3)
		return;

	auto drumScreen = dynamic_pointer_cast<DrumScreen>(mpc.screens->getScreenComponent("drum"));
	drumScreen->drum = i;

	if (mpc.getNote() < 35)
		mpc.setPadAndNote(program.lock()->getPadIndexFromNote(35), 35);

	if (redirectScreen.compare("") != 0)
	{
		ls.lock()->openScreen(redirectScreen);
		redirectScreen = "";
	}
	else {
		ls.lock()->openScreen("program-assign");
	}
	return;
}