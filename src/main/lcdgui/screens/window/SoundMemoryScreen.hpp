#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class SoundMemoryScreen final : public ScreenComponent
    {
    public:
        SoundMemoryScreen(Mpc &mpc, int layerIndex);
        void open() override;

    private:
        void displayFreeMemoryTime() const;
        void displayIndicator();
        void displayMegabytesInstalled() const;
    };
} // namespace mpc::lcdgui::screens::window
