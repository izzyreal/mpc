#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{

    class TransmitProgramChangesScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;

    public:
        TransmitProgramChangesScreen(mpc::Mpc &mpc, const int layerIndex);

    public:
        void open() override;

    private:
        void displayTransmitProgramChangesInThisTrack();

    private:
        bool transmitProgramChangesInThisTrack = false;

    public:
        bool isTransmitProgramChangesInThisTrackEnabled();
    };
} // namespace mpc::lcdgui::screens::window
