#include "VmpcFileInUseScreen.hpp"
using namespace mpc::lcdgui::screens::dialog;

VmpcFileInUseScreen::VmpcFileInUseScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "vmpc-file-in-use", layerIndex)
{
}

void VmpcFileInUseScreen::function(int i)
{

    switch (i)
    {
        case 3:
            openScreenById(ScreenId::VmpcDirectToDiskRecorderScreen);
            break;
    }
}
