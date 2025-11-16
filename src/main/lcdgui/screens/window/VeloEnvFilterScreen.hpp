#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VeloEnvFilterScreen final : public ScreenComponent
    {
    public:
        VeloEnvFilterScreen(Mpc &mpc, int layerIndex);

        void turnWheel(int i) override;

        void open() override;

    private:
        Velocity velo;
        void setVelo(Velocity);
        void displayVelo() const;
        void displayNote() const;
        void displayAttack();
        void displayDecay();
        void displayAmount() const;
        void displayVeloFreq() const;
    };
} // namespace mpc::lcdgui::screens::window
