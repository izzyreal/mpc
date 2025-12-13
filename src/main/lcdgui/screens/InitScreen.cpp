#include "InitScreen.hpp"

#include "Mpc.hpp"
#include "lcdgui/screens/UserScreen.hpp"
#include "lcdgui/screens/SetupScreen.hpp"

using namespace mpc::lcdgui::screens;

InitScreen::InitScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "init", layerIndex)
{
}

void InitScreen::function(int i)
{

    switch (i)
    {
        case 0:
            openScreenById(ScreenId::OthersScreen);
            break;
        case 2:
            openScreenById(ScreenId::VerScreen);
            break;
        case 5:
        {
            const auto userScreen = mpc.screens->get<ScreenId::UserScreen>();
            userScreen->resetPreferences();

            // const auto setupScreen =
            // mpc.screens->get<ScreenId::SetupScreen>();
            // setupScreen->resetPreferences();

            openScreenById(ScreenId::SequencerScreen);
            break;
        }
    }
}
