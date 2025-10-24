#include "SetupScreen.hpp"

using namespace mpc::lcdgui::screens;

SetupScreen::SetupScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "setup", layerIndex)
{
    resetPreferences();
}

void SetupScreen::open()
{
    displayDiskDevice();
    displayAutoLoadType();
}

void SetupScreen::setAutoLoadType(int i)
{
    if (i < 0 || i > 3)
    {
        return;
    }

    autoLoadType = i;
    displayAutoLoadType();
}

void SetupScreen::setDiskDevice(int i)
{
    if (i < 0 || i > 8)
    {
        return;
    }

    diskDevice = i;
    displayDiskDevice();
}

void SetupScreen::displayDiskDevice()
{
}

void SetupScreen::displayAutoLoadType()
{
}

void SetupScreen::resetPreferences()
{
    diskDevice = 0;
    autoLoadType = 0;
}
