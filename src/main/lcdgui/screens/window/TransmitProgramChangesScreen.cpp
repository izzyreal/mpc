#include "TransmitProgramChangesScreen.hpp"

#include <ui/sequencer/window/SequencerWindowGui.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

TransmitProgramChangesScreen::TransmitProgramChangesScreen(const int& layer)
	: ScreenComponent("transmitprogramchanges", layer)
{
}

void TransmitProgramChangesScreen::turnWheel(int i)
{
	init();
	auto swGui = mpc.getUis().lock()->getSequencerWindowGui(); 
	if (param.compare("inthistrack") == 0)
	{
		swGui->setTransmitProgramChangesInThisTrack(i > 0);
	}
}
