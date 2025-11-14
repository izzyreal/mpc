#pragma once
#include "lcdgui/ScreenComponent.hpp"

#include "lcdgui/screens/WithTimesAndNotes.hpp"

namespace mpc::lcdgui::screens::window
{
    class VmpcDirectToDiskRecorderScreen final : public ScreenComponent,
                                                 WithTimesAndNotes
    {
    public:
        VmpcDirectToDiskRecorderScreen(Mpc &, int layerIndex);

        void open() override;
        void turnWheel(int i) override;
        void function(int i) override;

        bool isOffline() const;
        int getSampleRate() const;
        int getRecord() const;

        bool seqLoopWasEnabled = false;
        bool songLoopWasEnabled = false;
        SequenceIndex sq{MinSequenceIndex};
        int song = 0;
        bool offline = false;

    private:
        const std::vector<std::string> recordNames{
            "SEQUENCE", "LOOP", "CUSTOM RANGE", "SONG", "JAM"};
        int record = 0;
        bool splitStereoIntoLeftAndRightChannel = true;
        int sampleRate = 0;
        void setRecord(int i);
        void setSq(SequenceIndex);
        void setSong(int i);
        void setOffline(bool b);
        void setSplitLR(bool b);
        void setSampleRate(int rate);

        void displayRate() const;
        void displaySong() const;
        void displayOffline() const;
        void displaySplitLR() const;
        void displayRecord() const;
        void displaySq() const;

        void displayTime() override;
        void displayNotes() override {} // We don't have notes in this screen
    };
} // namespace mpc::lcdgui::screens::window
