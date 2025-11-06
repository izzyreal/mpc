#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class VelocityModulationScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;

        VelocityModulationScreen(mpc::Mpc &mpc, int layerIndex);

        void open() override;
        void close() override;

        void update(Observable *observable, Message message) override;

    private:
        void displayVeloAttack();
        void displayVeloStart();
        void displayVeloLevel();
        void displayVelo();
        void displayNote();
    };
} // namespace mpc::lcdgui::screens::window
