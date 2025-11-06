#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VmpcRecordingFinishedScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        VmpcRecordingFinishedScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
