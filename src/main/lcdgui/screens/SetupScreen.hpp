#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class InitScreen;
}

namespace mpc::lcdgui::screens
{
    class SetupScreen final : public ScreenComponent
    {
    public:
        SetupScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        int diskDevice = 0;
        int autoLoadType = 0;

        void setDiskDevice(int i);
        void setAutoLoadType(int i);

        void displayDiskDevice() const;
        void displayAutoLoadType() const;

        void resetPreferences();

        friend class InitScreen;
    };
} // namespace mpc::lcdgui::screens
