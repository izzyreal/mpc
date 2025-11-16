#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VelocityModulationScreen final : public ScreenComponent
    {
    public:
        VelocityModulationScreen(Mpc &mpc, int layerIndex);

        void turnWheel(int i) override;

        void open() override;

    private:
        Velocity velo;
        void displayVeloAttack() const;
        void displayVeloStart() const;
        void displayVeloLevel() const;
        void displayVelo() const;
        void displayNote() const;

        void setVelo(Velocity);
    };
} // namespace mpc::lcdgui::screens::window
