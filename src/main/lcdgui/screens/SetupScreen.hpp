#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class InitScreen;
}

namespace mpc::lcdgui::screens
{
    class SetupScreen
        : public mpc::lcdgui::ScreenComponent
    {
    public:
        SetupScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

    private:
        int diskDevice = 0;
        int autoLoadType = 0;

        void setDiskDevice(int i);
        void setAutoLoadType(int i);

        void displayDiskDevice();
        void displayAutoLoadType();

        void resetPreferences();

        friend class InitScreen;
    };
} // namespace mpc::lcdgui::screens
