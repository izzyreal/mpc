#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::dialog
{
    class DeleteProgramScreen final : public ScreenComponent
    {
    public:
        void function(int i) override;
        void turnWheel(int i) override;

        DeleteProgramScreen(Mpc &mpc, int layerIndex);
        void open() override;

    private:
        int pgm = 0;

        void setPgm(int i);
        void displayPgm() const;
    };
} // namespace mpc::lcdgui::screens::dialog
