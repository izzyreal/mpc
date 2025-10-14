#include "VmpcResetKeyboardScreen.hpp"
#include "controller/ClientInputControllerBase.h"

#include <controls/KbMapping.hpp>

using namespace mpc::lcdgui::screens::window;

VmpcResetKeyboardScreen::VmpcResetKeyboardScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "vmpc-reset-keyboard", layerIndex)
{
}

void VmpcResetKeyboardScreen::function(int i)
{
    ScreenComponent::function(i);
    
    switch (i)
    {
        case 3:
            openScreen("vmpc-keyboard");
            break;
        case 4:
            mpc.inputController->getKbMapping()->initializeDefaults();
            openScreen("vmpc-keyboard");
            break;
    }
}
