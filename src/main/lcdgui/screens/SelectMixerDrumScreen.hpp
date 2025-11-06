#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class SelectMixerDrumScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        SelectMixerDrumScreen(Mpc &mpc, int layerIndex);

        void open() override;
    };
} // namespace mpc::lcdgui::screens
