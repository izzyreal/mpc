#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include "lcdgui/screens/WithTimesAndNotes.hpp"

namespace mpc::lcdgui::screens
{
    class EventsScreen final : public ScreenComponent, public WithTimesAndNotes
    {

    public:
        void function(int i) override;
        void turnWheel(int i) override;

        void setNote0(int i) override;

        EventsScreen(Mpc &mpc, int layerIndex);

        void open() override;

        void performCopy(int sourceStart, int sourceEnd, int toSequenceIndex,
                         int destStart, int toTrackIndex, bool copyModeMerge,
                         int copyCount, int copyNote0, int copyNote1) const;

        int toSq = 0;
        int tab = 0;
        int copies = 1;
        void setFromSq(int i) const;
        void setToSq(int i);

        const std::vector<std::string> tabNames{"events", "bars", "tr-move",
                                                "user"};

    private:
        void displayTime() override;
        void displayNotes() override;
        void displayDrumNotes() override;

        void displayStart() const;
        void displayCopies() const;
        void displayMode() const;
        void displayEdit() const;
        void displayMidiNotes() const;
        void displayToTr() const;
        void displayToSq() const;
        void displayFromTr() const;
        void displayFromSq() const;

        bool setNote1X = true;
        const std::vector<std::string> modeNames{"ADD VALUE", "SUB VALUE",
                                                 "MULTI VAL%", "SET TO VAL"};
        const std::vector<std::string> functionNames{"COPY", "DURATION",
                                                     "VELOCITY", "TRANSPOSE"};
        bool modeMerge = false;
        int editFunctionNumber = 0;
        int toTr = 0;
        int start = 0;
        int durationMode = 0;
        int velocityMode = 0;
        int transposeAmount = 0;
        int durationValue = 1;
        int velocityValue = 1;

        void setEdit(int i);
        void setFromTr(int i) const;
        void setToTr(int i);
        void setModeMerge(bool b);
        void setCopies(int i);
        void setDurationMode(int i);
        void setVelocityMode(int i);
        void setTransposeAmount(int i);
        void setDuration(int i);
        void setVelocityValue(int i);
        void setStart(int startTicks);
    };
} // namespace mpc::lcdgui::screens
