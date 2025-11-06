#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class DeleteAllSoundScreen : public mpc::lcdgui::ScreenComponent
    {
    public:
        void function(int i) override;

        DeleteAllSoundScreen(mpc::Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::dialog
