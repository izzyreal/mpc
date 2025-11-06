#include "VmpcResetKeyboardScreen.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"

using namespace mpc::lcdgui::screens::window;

VmpcResetKeyboardScreen::VmpcResetKeyboardScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-reset-keyboard", layerIndex)
{
}

void VmpcResetKeyboardScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::VmpcKeyboardScreen);
            break;
        case 4:
            mpc.clientEventController->getKeyboardBindings()
                ->initializeDefaults();
            openScreenById(ScreenId::VmpcKeyboardScreen);
            break;
    }
}
