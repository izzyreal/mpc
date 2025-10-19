#include "Mpc2000XlAllFileScreen.hpp"

#include <disk/AbstractDisk.hpp>

#include <lcdgui/screens/LoadScreen.hpp>
#include <lcdgui/screens/window/LoadASequenceFromAllScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

Mpc2000XlAllFileScreen::Mpc2000XlAllFileScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "mpc2000xl-all-file", layerIndex)
{
}

void Mpc2000XlAllFileScreen::function(int i)
{
	init();

	auto loadScreen = mpc.screens->get<LoadScreen>();

	switch (i)
	{
	case 2:
	{
        auto result = mpc.getDisk()->readSequencesFromAll2(loadScreen->getSelectedFile());
        
        if (result.has_value())
        {
            auto loadASequenceFromAllScreen = mpc.screens->get<LoadASequenceFromAllScreen>();
            loadASequenceFromAllScreen->sequencesFromAllFile = result.value();
            openScreen("load-a-sequence-from-all");
        }
        
		break;
	}
	case 3:
		openScreen("load");
		break;
	case 4:
	{
        auto on_success = [&](){openScreen("load");};
        mpc.getDisk()->readAll2(loadScreen->getSelectedFile(), on_success);
		break;
	}
	}
}
