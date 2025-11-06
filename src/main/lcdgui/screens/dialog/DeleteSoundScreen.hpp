#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens::dialog
{

    class DeleteSoundScreen final : public ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        DeleteSoundScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displaySnd() const;
    };
} // namespace mpc::lcdgui::screens::dialog
