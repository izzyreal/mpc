#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::window
{
    class VmpcRecordingFinishedScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;

        VmpcRecordingFinishedScreen(mpc::Mpc &mpc, const int layerIndex);
    };
} // namespace mpc::lcdgui::screens::window
