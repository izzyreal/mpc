#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{

    class InitPadAssignScreen : public mpc::lcdgui::ScreenComponent
    {

    private:
        void displayInitPadAssign();
        bool initPadAssignIsMaster = false;

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        InitPadAssignScreen(mpc::Mpc &mpc, int layerIndex);

        void open() override;
    };
} // namespace mpc::lcdgui::screens::window
