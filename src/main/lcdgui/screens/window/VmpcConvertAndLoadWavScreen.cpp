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

void VmpcConvertAndLoadWavScreen::setLoadRoutine(std::function<void()> loadRoutine)
{
    this->loadRoutine = loadRoutine;
}

void VmpcConvertAndLoadWavScreen::function(int i)
{
    switch (i)
    {
        case 3:
            openScreen(mpc.getLayeredScreen().lock()->getPreviousScreenName());
            break;
        case 4:
            loadRoutine();
            break;
    }
}
