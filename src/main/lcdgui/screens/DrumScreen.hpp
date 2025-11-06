#pragma once
#include "lcdgui/ScreenComponent.hpp"

namespace mpc::lcdgui::screens
{
    class DrumScreen final : public ScreenComponent
    {
    public:
        DrumScreen(Mpc &mpc, int layerIndex);
        void function(int f) override;
        void turnWheel(int i) override;
        void open() override;

        void setDrum(unsigned char i);
        unsigned char getDrum() const;
        void setPadToIntSound(bool b);
        bool isPadToIntSound() const;

    private:
        void displayCurrentVal() const;
        void displayDrum() const;
        void displayPadToInternalSound() const;
        void displayPgm() const;
        void displayPgmChange() const;
        void displayMidiVolume() const;

        bool padToInternalSound = true;
        unsigned char drum = 0;
    };
} // namespace mpc::lcdgui::screens
