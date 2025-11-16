#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VeloPitchScreen final : public ScreenComponent
    {

    public:
        VeloPitchScreen(Mpc &mpc, int layerIndex);
        void turnWheel(int i) override;
        void open() override;

    private:
        Velocity velo;
        void setVelo(Velocity);
        void displayVelo() const;
        void displayNote() const;
        void displayVeloPitch() const;
        void displayTune() const;
    };
} // namespace mpc::lcdgui::screens::window
