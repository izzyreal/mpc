#include "VmpcRecordingFinishedScreen.hpp"

#include "lcdgui/LayeredScreen.hpp"

using namespace mpc::lcdgui::screens::window;

VmpcRecordingFinishedScreen::VmpcRecordingFinishedScreen(Mpc &mpc,
                                                         const int layerIndex)
    : ScreenComponent(mpc, "vmpc-recording-finished", layerIndex)
{
}

void VmpcRecordingFinishedScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            ls.lock()->closeCurrentScreen();
            break;
        default:;
    }
}
