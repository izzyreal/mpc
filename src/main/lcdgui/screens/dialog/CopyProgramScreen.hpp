#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::dialog
{
    class CopyProgramScreen final : public ScreenComponent
    {
    public:
        void function(int i) override;
        void turnWheel(int increment) override;

        CopyProgramScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        ProgramIndex pgm0 = MinProgramIndex;
        ProgramIndex pgm1 = MinProgramIndex;

        void setPgm0(ProgramIndex);
        void setPgm1(ProgramIndex);

        void displayPgm0() const;
        void displayPgm1() const;

        void displayFunctionKeys();
    };
} // namespace mpc::lcdgui::screens::dialog
