#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include "lcdgui/screens/OpensNameScreen.hpp"

namespace mpc::lcdgui::screens::window
{
    class SequenceScreen
        : public mpc::lcdgui::ScreenComponent,
          public mpc::lcdgui::screens::OpensNameScreen
    {
    public:
        void openNameScreen() override;
        SequenceScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;
        void function(int i) override;
    };
} // namespace mpc::lcdgui::screens::window
