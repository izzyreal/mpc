#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{

    class PurgeScreen final : public ScreenComponent
    {

    public:
        void function(int f) override;

        PurgeScreen(Mpc &mpc, int layerIndex);

        void open() override;
    };
} // namespace mpc::lcdgui::screens
