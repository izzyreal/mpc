#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{

    class PasteEventScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        PasteEventScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
