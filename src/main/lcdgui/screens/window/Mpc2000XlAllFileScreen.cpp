#include "Mpc2000XlAllFileScreen.hpp"

#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/LoadASequenceFromAllScreen.hpp>

#include <disk/AllLoader.hpp>
#include <ui/disk/DiskGui.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace std;

Mpc2000XlAllFileScreen::Mpc2000XlAllFileScreen(const int layerIndex) 
	: ScreenComponent("mpc2000xl-all-file", layerIndex)
{
}

void Mpc2000XlAllFileScreen::function(int i)
{
	init();

	auto loadScreen = dynamic_pointer_cast<LoadScreen>(Screens::getScreenComponent("load"));

	switch (i)
	
	{
	case 2:
	{
		auto sequencesOnly = true;
		mpc::disk::AllLoader allLoader(loadScreen->getSelectedFile(), sequencesOnly);
		
		auto loadASequenceFromAllScreen = dynamic_pointer_cast<LoadASequenceFromAllScreen>(Screens::getScreenComponent("load-a-sequence-from-all"));
		loadASequenceFromAllScreen->sequencesFromAllFile = allLoader.getSequences();

		loadScreen->fileLoad = 0;
		
		ls.lock()->openScreen("load-a-sequence-from-all");
		break;
	}
	case 3:
		ls.lock()->openScreen("load");
		break;
	case 4:
	{
		auto sequencesOnly = false;
		mpc::disk::AllLoader allLoader(loadScreen->getSelectedFile(), sequencesOnly);
		ls.lock()->openScreen("sequencer");
		break;
	}
	}
}
