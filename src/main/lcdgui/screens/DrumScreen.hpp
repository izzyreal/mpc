#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/BusType.hpp"

namespace mpc::lcdgui::screens
{
    class DrumScreen final : public ScreenComponent
    {
    public:
        DrumScreen(Mpc &mpc, int layerIndex);
        void function(int f) override;
        void turnWheel(int i) override;
        void open() override;

        void setDrum(sequencer::BusType);
        sequencer::BusType getDrum() const;
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
        sequencer::BusType drum = sequencer::BusType::DRUM1;
    };
} // namespace mpc::lcdgui::screens
