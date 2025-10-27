#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{

    class PasteEventScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;

        PasteEventScreen(mpc::Mpc &mpc, const int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
