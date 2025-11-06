#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class DeleteAllProgramsScreen final : public ScreenComponent
    {
    public:
        void function(int j) override;

        DeleteAllProgramsScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::dialog
