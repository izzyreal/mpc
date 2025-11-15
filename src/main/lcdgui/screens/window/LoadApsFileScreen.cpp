#include "LoadApsFileScreen.hpp"
#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"

#include "disk/AbstractDisk.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadApsFileScreen::LoadApsFileScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "load-aps-file", layerIndex)
{
}

void LoadApsFileScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            ls->closeCurrentScreen();
            break;
        case 4:
        {
            const std::function on_success = [&]
            {
                ls->postToUiThread(
                    [ls = ls]
                    {
                        ls->closeRecentScreensUntilReachingLayer(0);
                    });
            };
            const auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
            mpc.getDisk()->readAps2(loadScreen->getSelectedFile(), on_success);
            break;
        }
        default:;
    }
}
