#pragma once

#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VmpcPhysicalSoundsScreen final : public ScreenComponent
    {
    public:
        VmpcPhysicalSoundsScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void turnWheel(int increment) override;

    private:
        void displayLevels() const;
    };
} // namespace mpc::lcdgui::screens::window
