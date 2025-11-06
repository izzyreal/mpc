#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{

    class PurgeScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int f) override;

        PurgeScreen(mpc::Mpc &mpc, int layerIndex);

        void open() override;
    };
} // namespace mpc::lcdgui::screens
