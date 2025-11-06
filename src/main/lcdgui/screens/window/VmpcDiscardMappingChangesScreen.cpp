#include "VmpcDiscardMappingChangesScreen.hpp"
#include "lcdgui/LayeredScreen.hpp"

using namespace mpc::lcdgui::screens::window;

VmpcDiscardMappingChangesScreen::VmpcDiscardMappingChangesScreen(
    Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-discard-mapping-changes", layerIndex)
{
}

void VmpcDiscardMappingChangesScreen::close()
{
    nextScreen = "sequencer";
}
void VmpcDiscardMappingChangesScreen::setNextScreen(std::string nextScreenToUse)
{
    nextScreen = nextScreenToUse;
}

void VmpcDiscardMappingChangesScreen::function(const int i)
{
    ScreenComponent::function(i);

    switch (i)
    {
        case 3:
            // stay
            ls->openScreen(stayScreen);
            break;
        case 4:
            // discard and leave
            discardAndLeave();
            ls->openScreen(nextScreen);
            break;
        case 5:
            // save and leave
            saveAndLeave();
            ls->openScreen(nextScreen);
            break;
    }
}
