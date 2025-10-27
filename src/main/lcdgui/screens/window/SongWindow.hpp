#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class SongWindow : public mpc::lcdgui::ScreenComponent,
                       public mpc::lcdgui::screens::OpensNameScreen
    {

    public:
        SongWindow(mpc::Mpc &mpc, const int layerIndex);

    public:
        void open() override;
        void openNameScreen() override;
        void function(int i) override;
    };
} // namespace mpc::lcdgui::screens::window
