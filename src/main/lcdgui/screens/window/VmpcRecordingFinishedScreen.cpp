#include "VmpcRecordingFinishedScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace std;

VmpcRecordingFinishedScreen::VmpcRecordingFinishedScreen(const int layerIndex)
	: ScreenComponent("vmpc-recording-finished", layerIndex)
{
}

void VmpcRecordingFinishedScreen::function(int i)
{
    init();

	switch (i)
    {
    case 3:
        ls.lock()->openScreen("sequencer");
        break;
    }
}
