#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class CantFindFileScreen final : public ScreenComponent
    {
    public:
        void function(int i) override;
        void open() override;

        CantFindFileScreen(Mpc &mpc, int layerIndex);

        bool skipAll = false;
        bool waitingForUser = false;
        std::string fileName;
    };
} // namespace mpc::lcdgui::screens::window
