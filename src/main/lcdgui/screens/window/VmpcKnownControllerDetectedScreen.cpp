#include "VmpcKnownControllerDetectedScreen.hpp"
#include "lcdgui/screens/VmpcMidiScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;

VmpcKnownControllerDetectedScreen::VmpcKnownControllerDetectedScreen(mpc::Mpc &m, int layer)
: ScreenComponent(m, "vmpc-known-controller-detected", layer)
{
    auto label0 = std::make_shared<Label>(mpc, "line0", "It looks like you have connected", 24, 10, 32 * 6);
    addChild(label0);
    auto label1 = std::make_shared<Label>(mpc, "line1", "", 24, 19, 32 * 6);
    addChild(label1);
    auto label2 = std::make_shared<Label>(mpc, "line2", "Do you want to switch active MIDI", 24, 28, 32 * 6);
    addChild(label2);
    auto label3 = std::make_shared<Label>(mpc, "line3", "mapping to this controller?", 24, 37, 32 * 6);
    addChild(label3);
}

void VmpcKnownControllerDetectedScreen::function(int i)
{
    auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");

    switch (i) {
        case 1:
            // NO
            openScreen(ls->getPreviousScreenName());
            break;
        case 2:
            // YES
            vmpcMidiScreen->shouldSwitch.store(true);
            openScreen(ls->getPreviousScreenName());
            break;
        case 3:
            // NEVER
            shouldAutoSwitch = 0;
            openScreen(ls->getPreviousScreenName());
            break;
        case 4:
            // ALWAYS
            shouldAutoSwitch = 2;
            vmpcMidiScreen->shouldSwitch.store(true);
            openScreen(ls->getPreviousScreenName());
            break;
        default:
            break;
    }
}

void VmpcKnownControllerDetectedScreen::displayMessage()
{
    findLabel("line1")->setText(controllerName);
}

void VmpcKnownControllerDetectedScreen::open()
{
    if (shouldAutoSwitch == 0)
    {
        openScreen(ls->getPreviousScreenName());
        return;
    }
    else if (shouldAutoSwitch == 2)
    {
        auto vmpcMidiScreen = mpc.screens->get<VmpcMidiScreen>("vmpc-midi");
        vmpcMidiScreen->shouldSwitch.store(true);
        openScreen(ls->getPreviousScreenName());
        return;
    }

    displayMessage();
}
