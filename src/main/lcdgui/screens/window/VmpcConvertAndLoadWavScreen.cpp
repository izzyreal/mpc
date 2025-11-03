#include "VmpcConvertAndLoadWavScreen.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

VmpcConvertAndLoadWavScreen::VmpcConvertAndLoadWavScreen(mpc::Mpc &mpc,
                                                         const int layerIndex)
    : ScreenComponent(mpc, "vmpc-convert-and-load-wav", layerIndex)
{
}

void VmpcConvertAndLoadWavScreen::setLoadRoutine(
    std::function<void()> newLoadRoutine)
{
    loadRoutine = newLoadRoutine;
}

void VmpcConvertAndLoadWavScreen::function(int i)
{
    switch (i)
    {
        case 3:
            ls->closeCurrentScreen();
            break;
        case 4:
            loadRoutine();
            break;
    }
}
