#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens::dialog
{
    class CopySongScreen : public mpc::lcdgui::ScreenComponent
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        CopySongScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

    private:
        int song1 = 0;
        void setSong1(int i);

        void displaySong0();
        void displaySong1();
    };
} // namespace mpc::lcdgui::screens::dialog
