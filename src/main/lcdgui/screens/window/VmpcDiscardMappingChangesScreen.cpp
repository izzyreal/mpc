#include "VmpcDiscardMappingChangesScreen.hpp"

#include <controls/KbMapping.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace std;

VmpcDiscardMappingChangesScreen::VmpcDiscardMappingChangesScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "vmpc-discard-mapping-changes", layerIndex)
{
}

void VmpcDiscardMappingChangesScreen::function(int i)
{
    ScreenComponent::function(i);
    
    switch (i)
    {
        case 3:
            // stay
            openScreen("vmpc-keyboard");
            break;
        case 4:
            // discard and leave
            mpc.getControls().lock()->getKbMapping().lock()->importMapping();
            openScreen("sequencer");
            break;
        case 5:
            // save and leave
            mpc.getControls().lock()->getKbMapping().lock()->exportMapping();
            openScreen("sequencer");
            break;
    }
}
