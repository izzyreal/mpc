#pragma once
#include "lcdgui/ScreenComponent.hpp"
#include "lcdgui/screens/WithTimesAndNotes.hpp"

namespace mpc::lcdgui::screens::window
{
    class TimingCorrectScreen final : public ScreenComponent,
                                      public WithTimesAndNotes
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        void setNote0(int i) override;

        TimingCorrectScreen(Mpc &mpc, int layerIndex);

        void open() override;

    private:
        void displayNoteValue();
        void displaySwing() const;
        void displayShiftTiming() const;
        void displayAmount() const;

    protected:
        void displayNotes() override;
        void displayTime() override;

    private:
        const std::vector<std::string> noteValueNames = {
            "OFF", "1/8", "1/8(3)", "1/16", "1/16(3)", "1/32", "1/32(3)"};
        const std::vector<unsigned char> noteValueLengthsInTicks{1,  48, 32, 24,
                                                                 16, 12, 8};
        bool shiftTimingLater = false;
        int swing = 50;
        int amount = 0;
        int noteValue = 3;

    public:
        unsigned char getNoteValueLengthInTicks() const;
        int getNoteValue() const;
        void setNoteValue(int i);
        int getSwing() const;
        int getAmount() const;
        int isShiftTimingLater() const;

    private:
        void setAmount(int i);
        void setShiftTimingLater(bool b);
        void setSwing(int i);
    };
} // namespace mpc::lcdgui::screens::window
