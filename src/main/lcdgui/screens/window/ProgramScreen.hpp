#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class ProgramScreen final : public ScreenComponent, public OpensNameScreen
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;
        void openNameScreen() override;

        ProgramScreen(Mpc &mpc, int layerIndex);
        void open() override;

    private:
        void displayProgramName() const;
        void displayMidiProgramChange() const;
    };
} // namespace mpc::lcdgui::screens::window
