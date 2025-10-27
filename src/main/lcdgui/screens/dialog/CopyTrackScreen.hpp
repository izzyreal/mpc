#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class CopyTrackScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        CopyTrackScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

    private:
        int tr0 = 0;
        int tr1 = 0;
        void setTr0(int i);
        void setTr1(int i);

        void displayTr0();
        void displayTr1();
    };
} // namespace mpc::lcdgui::screens::dialog
