#include "Mpc2000XlAllFileScreen.hpp"

#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/LoadASequenceFromAllScreen.hpp>
#include <lcdgui/screens/dialog2/PopupScreen.hpp>

#include <disk/AllLoader.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens::dialog2;
using namespace mpc::lcdgui::screens;
using namespace std;

Mpc2000XlAllFileScreen::Mpc2000XlAllFileScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "mpc2000xl-all-file", layerIndex)
{
}

void Mpc2000XlAllFileScreen::function(int i)
{
	init();

	auto loadScreen = mpc.screens->get<LoadScreen>("load");

	switch (i)
	
	{
	case 2:
	{
		auto sequencesOnly = true;
		
		try
		{
			mpc::disk::AllLoader allLoader(mpc, loadScreen->getSelectedFile().get(), sequencesOnly);
			auto loadASequenceFromAllScreen = mpc.screens->get<LoadASequenceFromAllScreen>("load-a-sequence-from-all");
			loadASequenceFromAllScreen->sequencesFromAllFile = allLoader.getSequences();

			loadScreen->fileLoad = 0;

			openScreen("load-a-sequence-from-all");
		}
		catch (const exception& e)
		{
			auto popupScreen = mpc.screens->get<PopupScreen>("popup");
			popupScreen->setText("Wrong file format");
			popupScreen->returnToScreenAfterInteraction("load");
			mpc.getLayeredScreen().lock()->openScreen("popup");
		}
		break;
	}
	case 3:
		openScreen("load");
		break;
	case 4:
	{
		auto sequencesOnly = false;
		try
		{
			mpc::disk::AllLoader allLoader(mpc, loadScreen->getSelectedFile().get(), sequencesOnly);
			openScreen("sequencer");
		}
		catch (const exception& e)
		{
			auto popupScreen = mpc.screens->get<PopupScreen>("popup");
			popupScreen->setText("Wrong file format");
			popupScreen->returnToScreenAfterInteraction("load");
			mpc.getLayeredScreen().lock()->openScreen("popup");
		}
		break;
	}
	}
}
