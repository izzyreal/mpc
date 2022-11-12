#include "VmpcRecordingFinishedScreen.hpp"

using namespace mpc::lcdgui::screens::window;

VmpcRecordingFinishedScreen::VmpcRecordingFinishedScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "vmpc-recording-finished", layerIndex)
{
}

void VmpcRecordingFinishedScreen::function(int i)
{
    init();

	switch (i)
    {
    case 3:
        openScreen("sequencer");
        break;
    }
}
