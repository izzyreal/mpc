#include "VmpcResetKeyboardScreen.hpp"
#include "Mpc.hpp"
#include "controller/ClientEventController.hpp"
#include "lcdgui/screens/VmpcKeyboardScreen.hpp"

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
            ls.lock()->closeCurrentScreen();
            std::dynamic_pointer_cast<screens::VmpcKeyboardScreen>(
                ls.lock()->getCurrentScreen())
                ->initializeDefaults();
            break;
        default:;
    }
}
