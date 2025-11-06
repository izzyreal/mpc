#pragma once
#include <lcdgui/ScreenComponent.hpp>

namespace mpc::lcdgui::screens
{
    class DrumScreen : public mpc::lcdgui::ScreenComponent
    {
    public:
        DrumScreen(mpc::Mpc &mpc, int layerIndex);
        void function(int f) override;
        void turnWheel(int i) override;
        void open() override;

        void setDrum(unsigned char i);
        unsigned char getDrum();
        void setPadToIntSound(bool b);
        bool isPadToIntSound();

    private:
        void displayCurrentVal();
        void displayDrum();
        void displayPadToInternalSound();
        void displayPgm();
        void displayPgmChange();
        void displayMidiVolume();

        bool padToInternalSound = true;
        unsigned char drum = 0;
    };
} // namespace mpc::lcdgui::screens
