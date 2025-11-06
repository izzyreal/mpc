#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class DeleteAllSongScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        DeleteAllSongScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::dialog
