#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class TransmitProgramChangesScreen final : public ScreenComponent
    {
    public:
        TransmitProgramChangesScreen(Mpc &mpc, int layerIndex);

        void turnWheel(int i) override;
        void open() override;

    private:
        void displayTransmitProgramChangesInThisTrack() const;
    };
} // namespace mpc::lcdgui::screens::window
