#include "VmpcDiscardMappingChangesScreen.hpp"

using namespace mpc::lcdgui::screens::window;

VmpcDiscardMappingChangesScreen::VmpcDiscardMappingChangesScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-discard-mapping-changes", layerIndex)
{
}

void VmpcDiscardMappingChangesScreen::close()
{
    nextScreen = "sequencer";
}

void VmpcDiscardMappingChangesScreen::function(int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 3:
            // stay
            mpc.getLayeredScreen()->openScreen(stayScreen);
            break;
        case 4:
            // discard and leave
            discardAndLeave();
            mpc.getLayeredScreen()->openScreen(nextScreen);
            break;
        case 5:
            // save and leave
            saveAndLeave();
            mpc.getLayeredScreen()->openScreen(nextScreen);
            break;
    }
}
