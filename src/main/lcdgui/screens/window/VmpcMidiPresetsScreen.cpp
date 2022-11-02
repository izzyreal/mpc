#include "VmpcMidiPresetsScreen.hpp"

using namespace mpc::lcdgui::screens::window;

VmpcMidiPresetsScreen::VmpcMidiPresetsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "vmpc-midi-presets", layerIndex)
{
}

void VmpcMidiPresetsScreen::function(int i)
{
    ScreenComponent::function(i);
    
    switch (i)
    {
        case 3:
            openScreen("vmpc-midi");
            break;
        case 4:
            // save
            break;
        case 5:
            // load
            break;
    }
}
