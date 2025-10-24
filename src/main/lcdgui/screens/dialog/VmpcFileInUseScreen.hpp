#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class VmpcFileInUseScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;

        VmpcFileInUseScreen(mpc::Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::dialog
