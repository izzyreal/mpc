#include "VmpcWarningSettingsIgnoredScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;

VmpcWarningSettingsIgnoredScreen::VmpcWarningSettingsIgnoredScreen(mpc::Mpc &m,
                                                                   int layer)
    : ScreenComponent(m, "vmpc-warning-settings-ignored", layer)
{
}

void VmpcWarningSettingsIgnoredScreen::function(int i)
{
    if (i == 3)
    {
        mpc.getLayeredScreen()->openScreen<VmpcSettingsScreen>();
        ls->setFocus("midi-control-mode");
    }
    else if (i == 4)
    {
        mpc.getLayeredScreen()->openPreviousScreen();
    }
}
