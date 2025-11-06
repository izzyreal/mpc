#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class InitPadAssignScreen final : public ScreenComponent
    {

    private:
        void displayInitPadAssign() const;
        bool initPadAssignIsMaster = false;

    public:
        void turnWheel(int i) override;
        void function(int i) override;

        InitPadAssignScreen(Mpc &mpc, int layerIndex);

        void open() override;
    };
} // namespace mpc::lcdgui::screens::window
