#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class SoundMemoryScreen
        : public mpc::lcdgui::ScreenComponent
    {
    public:
        SoundMemoryScreen(mpc::Mpc &mpc, const int layerIndex);
        void open() override;

    private:
        void displayFreeMemoryTime();
        void displayIndicator();
        void displayMegabytesInstalled();
    };
} // namespace mpc::lcdgui::screens::window
