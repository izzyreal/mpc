#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{

    class TrackScreen : public mpc::lcdgui::ScreenComponent,
                        public mpc::lcdgui::screens::OpensNameScreen
    {
    public:
        void openNameScreen() override;
        void function(int i) override;
        void open() override;

        TrackScreen(mpc::Mpc &mpc, const int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
