#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{

    class EraseAllOffTracksScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;

        EraseAllOffTracksScreen(mpc::Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
