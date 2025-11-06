#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VelocityModulationScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;

        VelocityModulationScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void close() override;

        void update(Observable *observable, Message message) override;

    private:
        void displayVeloAttack() const;
        void displayVeloStart() const;
        void displayVeloLevel() const;
        void displayVelo() const;
        void displayNote() const;
    };
} // namespace mpc::lcdgui::screens::window
