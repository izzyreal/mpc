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
        // Should be set based on hitting a drum pad, but is currently unimplemented
        // so we display a fixed value.
        unsigned int velo = 127;
        void setVelo(int i);
        void displayVelo() const;
        void displayNote() const;
        void displayAttack();
        void displayDecay();
        void displayAmount() const;
        void displayVeloFreq() const;
    };
} // namespace mpc::lcdgui::screens::window
