#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{

    class NumberOfZonesScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        NumberOfZonesScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

    private:
        void displayNumberOfZones();

        int numberOfZones = 0;

        void setNumberOfZones(int i);
    };
} // namespace mpc::lcdgui::screens::window
