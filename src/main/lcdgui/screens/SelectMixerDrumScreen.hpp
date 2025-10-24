#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class SelectMixerDrumScreen
        : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;

        SelectMixerDrumScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;
    };
} // namespace mpc::lcdgui::screens
