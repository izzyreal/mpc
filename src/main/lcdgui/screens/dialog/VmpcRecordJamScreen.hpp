#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class VmpcRecordJamScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        VmpcRecordJamScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::dialog
