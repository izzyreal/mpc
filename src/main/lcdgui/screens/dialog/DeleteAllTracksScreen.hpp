#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class DeleteAllTracksScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        DeleteAllTracksScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::dialog
