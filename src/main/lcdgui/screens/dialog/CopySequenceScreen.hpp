#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class CopySequenceScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        CopySequenceScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        int sq0 = 0;
        int sq1 = 0;
        void setSq0(int i);
        void setSq1(int i);

        void displaySq0();
        void displaySq1();
    };
} // namespace mpc::lcdgui::screens::dialog
