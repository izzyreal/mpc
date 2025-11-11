#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class TransmitProgramChangesScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;

        TransmitProgramChangesScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displayTransmitProgramChangesInThisTrack() const;

        bool transmitProgramChangesInThisTrack = false;

    public:
        bool isTransmitProgramChangesInThisTrackEnabled() const;
    };
} // namespace mpc::lcdgui::screens::window
