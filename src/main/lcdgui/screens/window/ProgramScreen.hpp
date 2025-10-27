#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class ProgramScreen : public mpc::lcdgui::ScreenComponent,
                          public mpc::lcdgui::screens::OpensNameScreen
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;
        void openNameScreen() override;

        ProgramScreen(mpc::Mpc &mpc, const int layerIndex);
        void open() override;

    private:
        void displayProgramName();
        void displayMidiProgramChange();
    };
} // namespace mpc::lcdgui::screens::window
