#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class DeleteAllSoundScreen final : public ScreenComponent
    {
    public:
        void function(int i) override;

        DeleteAllSoundScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::dialog
