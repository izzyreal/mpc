#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class EraseAllOffTracksScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        EraseAllOffTracksScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
