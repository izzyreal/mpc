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
        void displayVeloAttack() const;
        void displayVeloStart() const;
        void displayVeloLevel() const;
        void displayVelo() const;
        void displayNote() const;
    };
} // namespace mpc::lcdgui::screens::window
