#include "VmpcResetKeyboardScreen.hpp"

#include <controls/KbMapping.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

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
            mpc.getControls().lock()->getKbMapping().lock()->importMapping();
            openScreen("vmpc-keyboard");
            break;
    }
}
