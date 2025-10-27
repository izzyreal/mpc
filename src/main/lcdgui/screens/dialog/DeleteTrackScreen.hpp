#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class DeleteTrackScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        DeleteTrackScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

    private:
        int tr = 0;
        void setTr(int i);
        void displayTr();
    };
} // namespace mpc::lcdgui::screens::dialog
