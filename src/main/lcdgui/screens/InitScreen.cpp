#include "InitScreen.hpp"

#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/SetupScreen.hpp"

using namespace mpc::lcdgui::screens;

InitScreen::InitScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "init", layerIndex)
{
}

void InitScreen::function(int i)
{

    switch (i)
    {
        case 0:
            mpc.getLayeredScreen()->openScreen<OthersScreen>();
            break;
        case 2:
            mpc.getLayeredScreen()->openScreen<VerScreen>();
            break;
        case 5:
        {
            auto userScreen = mpc.screens->get<UserScreen>();
            userScreen->resetPreferences();

            auto setupScreen = mpc.screens->get<SetupScreen>();
            setupScreen->resetPreferences();

            mpc.getLayeredScreen()->openScreen<SequencerScreen>();
            break;
        }
    }
}
