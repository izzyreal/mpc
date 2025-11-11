#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class SongWindow final : public ScreenComponent, public OpensNameScreen
    {

    public:
        SongWindow(Mpc &mpc, int layerIndex);

        void open() override;
        void openNameScreen() override;
        void function(int i) override;
    };
} // namespace mpc::lcdgui::screens::window
