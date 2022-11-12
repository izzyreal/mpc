#include "VmpcWarningSettingsIgnoredScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;

VmpcWarningSettingsIgnoredScreen::VmpcWarningSettingsIgnoredScreen(mpc::Mpc &m, int layer)
: ScreenComponent(m, "vmpc-warning-settings-ignored", layer)
{
}

void VmpcWarningSettingsIgnoredScreen::function(int i)
{
    if (i == 3)
    {
        openScreen("vmpc-settings");
        ls->setFocus("midi-control-mode");
    }
    else if (i == 4)
    {
        openScreen(ls->getPreviousScreenName());
    }
}
