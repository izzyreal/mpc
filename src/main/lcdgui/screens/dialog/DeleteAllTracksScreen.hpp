#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class DeleteAllTracksScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;

        DeleteAllTracksScreen(mpc::Mpc &mpc, const int layerIndex);
    };
} // namespace mpc::lcdgui::screens::dialog
