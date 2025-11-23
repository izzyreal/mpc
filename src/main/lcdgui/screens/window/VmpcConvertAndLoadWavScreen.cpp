#include "VmpcConvertAndLoadWavScreen.hpp"

#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

VmpcConvertAndLoadWavScreen::VmpcConvertAndLoadWavScreen(Mpc &mpc,
                                                         const int layerIndex)
    : ScreenComponent(mpc, "vmpc-convert-and-load-wav", layerIndex)
{
}

void VmpcConvertAndLoadWavScreen::setLoadRoutine(
    const std::function<void()> &newLoadRoutine)
{
    loadRoutine = newLoadRoutine;
}

void VmpcConvertAndLoadWavScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            ls.lock()->closeCurrentScreen();
            break;
        case 4:
            loadRoutine();
            break;
    }
}
