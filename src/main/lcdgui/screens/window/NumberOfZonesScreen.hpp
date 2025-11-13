#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class NumberOfZonesScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        NumberOfZonesScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displayNumberOfZones() const;

        int zoneCount = 0;

        void setNumberOfZones(int i);
    };
} // namespace mpc::lcdgui::screens::window
