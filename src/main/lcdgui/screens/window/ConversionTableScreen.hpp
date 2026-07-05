#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include <cstddef>

namespace mpc::lcdgui::screens::window
{
    class ConversionTableScreen final : public ScreenComponent
    {
    public:
        ConversionTableScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void turnWheel(int i) override;
        void function(int i) override;

    private:
        size_t selectedMpc60PadIndex = 0;

        void displayPad() const;
        void displayBecomesNote() const;
    };
} // namespace mpc::lcdgui::screens::window
