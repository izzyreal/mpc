#include "TransmitProgramChangesScreen.hpp"

using namespace mpc::lcdgui::screens::window;

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
	if (param == "inthistrack")
	{
		transmitProgramChangesInThisTrack = i > 0;
		displayTransmitProgramChangesInThisTrack();
	}
}

void TransmitProgramChangesScreen::displayTransmitProgramChangesInThisTrack()
{
	findField("inthistrack")->setText(transmitProgramChangesInThisTrack ? "YES" : "NO");
}

bool TransmitProgramChangesScreen::isTransmitProgramChangesInThisTrackEnabled()
{
	return transmitProgramChangesInThisTrack;
}
