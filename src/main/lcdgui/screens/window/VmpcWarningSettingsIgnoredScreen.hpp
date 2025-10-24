#pragma once

#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::window
{
    class VmpcWarningSettingsIgnoredScreen : public ScreenComponent
    {
    public:
        VmpcWarningSettingsIgnoredScreen(mpc::Mpc &, int layer);
        void function(int) override;
    };
} // namespace mpc::lcdgui::screens::window