#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class VmpcFileInUseScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        VmpcFileInUseScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::dialog
