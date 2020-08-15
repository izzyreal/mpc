#include "PurgeScreen.hpp"

#include "SelectDrumScreen.hpp"

using namespace mpc::lcdgui::screens;
using namespace std;

PurgeScreen::PurgeScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "purge", layerIndex)
{
}

void PurgeScreen::open()
{
	findLabel("value").lock()->setTextPadded(sampler.lock()->getUnusedSampleCount(), " ");
	ls.lock()->setFunctionKeysArrangement(sampler.lock()->getUnusedSampleCount() > 0 ? 1 : 0);
}

void PurgeScreen::function(int f)
{
	init();
	
	switch (f)
	{
	case 0:
		ls.lock()->openScreen("program-assign");
		break;
	case 1:
		ls.lock()->openScreen("program-params");
		break;
	case 2:
		ls.lock()->openScreen("drum");
		break;
	case 3:
	{
		auto selectDrumScreen = dynamic_pointer_cast<SelectDrumScreen>(mpc.screens->getScreenComponent("select-drum"));
		selectDrumScreen->redirectScreen = "purge";
		ls.lock()->openScreen("select-drum");
		break;
	}
	case 5:
		sampler.lock()->purge();
		open();
		break;
	}
}
