#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{

    class TrackScreen final : public ScreenComponent, public OpensNameScreen
    {
    public:
        void openNameScreen() override;
        void function(int i) override;
        void open() override;

        TrackScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
