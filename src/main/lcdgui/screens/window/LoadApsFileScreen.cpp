#include "LoadApsFileScreen.hpp"
#include "Mpc.hpp"
#include "lcdgui/LayeredScreen.hpp"
#include "lcdgui/screens/LoadScreen.hpp"

#include "disk/AbstractDisk.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::lcdgui::screens;

LoadApsFileScreen::LoadApsFileScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "load-aps-file", layerIndex)
{
}

void LoadApsFileScreen::function(int i)
{
    switch (i)
    {
        case 3:
            ls->closeCurrentScreen();
            break;
        case 4:
        {
            std::function<void()> on_success = [&]()
            {
                ls->closeRecentScreensUntilReachingLayer(0);
            };
            auto loadScreen = mpc.screens->get<ScreenId::LoadScreen>();
            mpc.getDisk()->readAps2(loadScreen->getSelectedFile(), on_success);
            break;
        }
    }
}
