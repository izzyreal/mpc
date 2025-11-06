#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class DeleteAllSequencesScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;

        DeleteAllSequencesScreen(Mpc &mpc, int layerIndex);
    };
} // namespace mpc::lcdgui::screens::dialog
