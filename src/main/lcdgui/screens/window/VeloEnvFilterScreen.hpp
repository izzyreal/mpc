#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VeloEnvFilterScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;

        VeloEnvFilterScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void close() override;

        void update(Observable *observable, Message message) override;

    private:
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
