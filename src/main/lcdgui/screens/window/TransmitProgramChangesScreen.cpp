#include "TransmitProgramChangesScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace std;

TransmitProgramChangesScreen::TransmitProgramChangesScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "transmit-program-changes", layerIndex)
{
}

void TransmitProgramChangesScreen::open()
{
	displayTransmitProgramChangesInThisTrack();
}

void TransmitProgramChangesScreen::turnWheel(int i)
{
	init();
	if (param.compare("inthistrack") == 0)
	{
		transmitProgramChangesInThisTrack = i > 0;
		displayTransmitProgramChangesInThisTrack();
	}
}

void TransmitProgramChangesScreen::displayTransmitProgramChangesInThisTrack()
{
	findField("inthistrack").lock()->setText(transmitProgramChangesInThisTrack ? "YES" : "NO");
}

bool TransmitProgramChangesScreen::isTransmitProgramChangesInThisTrackEnabled()
{
	return transmitProgramChangesInThisTrack;
}
