#pragma once
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/screens/WithTimesAndNotes.hpp>

namespace mpc::lcdgui::screens::window
{
    class TimingCorrectScreen : public mpc::lcdgui::ScreenComponent,
                                public mpc::lcdgui::screens::WithTimesAndNotes
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        void setNote0(int i) override;

        TimingCorrectScreen(mpc::Mpc &mpc, const int layerIndex);

        void open() override;

    private:
        void displayNoteValue();
        void displaySwing();
        void displayShiftTiming();
        void displayAmount();

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
        unsigned char getNoteValueLengthInTicks();
        int getNoteValue();
        void setNoteValue(int i);
        int getSwing();
        int getAmount();
        int isShiftTimingLater();

    private:
        void setAmount(int i);
        void setShiftTimingLater(bool b);
        void setSwing(int i);
    };
} // namespace mpc::lcdgui::screens::window
