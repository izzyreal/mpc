#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::dialog
{
    class CopyProgramScreen final : public ScreenComponent
    {
    public:
        void function(int i) override;
        void turnWheel(int i) override;

        CopyProgramScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        int pgm0 = 0;
        int pgm1 = 0;

        void setPgm0(int i);
        void setPgm1(int i);

        void displayPgm0() const;
        void displayPgm1() const;

        void displayFunctionKeys();
    };
} // namespace mpc::lcdgui::screens::dialog
