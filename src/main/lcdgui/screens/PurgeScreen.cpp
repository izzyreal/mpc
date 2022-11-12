#include "PurgeScreen.hpp"

#include "SelectDrumScreen.hpp"

using namespace mpc::lcdgui::screens;

PurgeScreen::PurgeScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "purge", layerIndex)
{
}

void PurgeScreen::open()
{
	findLabel("value")->setTextPadded(sampler->getUnusedSampleCount(), " ");
	ls.lock()->setFunctionKeysArrangement(sampler->getUnusedSampleCount() > 0 ? 1 : 0);
}

void PurgeScreen::function(int f)
{
	init();
	
	switch (f)
	{
	case 0:
		openScreen("program-assign");
		break;
	case 1:
		openScreen("program-params");
		break;
	case 2:
		openScreen("drum");
		break;
	case 3:
	{
		auto selectDrumScreen = mpc.screens->get<SelectDrumScreen>("select-drum");
		selectDrumScreen->redirectScreen = "purge";
		openScreen("select-drum");
		break;
	}
	case 5:
		sampler->purge();
		open();
		break;
	}
}
