#include "VmpcConvertAndLoadWavScreen.hpp"

#include <lcdgui/screens/LoadScreen.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;
using namespace mpc::controls;
using namespace std;

VmpcConvertAndLoadWavScreen::VmpcConvertAndLoadWavScreen(mpc::Mpc& mpc, const int layerIndex)
: ScreenComponent(mpc, "vmpc-convert-and-load-wav", layerIndex)
{
}

void VmpcConvertAndLoadWavScreen::function(int i)
{
    switch (i)
    {
        case 3:
            openScreen("load");
            break;
        case 4:
            auto disk = mpc.getDisk().lock();
            auto loadScreen = mpc.screens->get<LoadScreen>("load");
            const bool shouldBeConverted = true;
            loadScreen->loadSound(shouldBeConverted);
            break;
    }
}
