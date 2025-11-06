#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{

    class DeleteSoundScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        DeleteSoundScreen(mpc::Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displaySnd();
    };
} // namespace mpc::lcdgui::screens::dialog
