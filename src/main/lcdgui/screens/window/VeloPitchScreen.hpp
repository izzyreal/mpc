#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VeloPitchScreen final : public ScreenComponent
    {

    public:
        void turnWheel(int i) override;

        VeloPitchScreen(Mpc &mpc, int layerIndex);

        void open() override;
        void close() override;

        void update(Observable *observable, Message message) override;

    private:
        void displayVelo() const;
        void displayNote() const;
        void displayVeloPitch() const;
        void displayTune() const;
    };
} // namespace mpc::lcdgui::screens::window
