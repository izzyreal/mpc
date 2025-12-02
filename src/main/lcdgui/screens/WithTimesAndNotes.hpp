#pragma once
#include "IntTypes.hpp"

namespace mpc
{
    class Mpc;
}

namespace mpc::sequencer
{
    class Sequence;
    class Track;
} // namespace mpc::sequencer

namespace mpc::lcdgui::screens
{
    class WithTimesAndNotes
    {
    protected:
        int time0 = 0;
        int time1 = 0;
        DrumNoteNumber drumNoteNumber{AllDrumNotes};
        NoteNumber midiNote0{MinNoteNumber};
        NoteNumber midiNote1{MaxNoteNumber};

    public:
        virtual ~WithTimesAndNotes() = default;
        void setTime0(int newTime0);
        void setTime1(int newTime1);

        virtual void setDrumNote(DrumNoteNumber);
        void setMidiNote0(NoteNumber);
        void setMidiNote1(NoteNumber);

    protected:
        virtual void displayNotes() = 0;
        virtual void displayDrumNotes()
        {
            displayNotes();
        }
        virtual void displayTime() = 0;

    public:
        bool checkAllTimes(Mpc &mpc, int notch,
                           const sequencer::Sequence *sequence = nullptr);
        bool checkAllTimesAndNotes(Mpc &mpc, int notch,
                              const sequencer::Sequence *sequence = nullptr,
                                   const sequencer::Track *track = nullptr);
    };
} // namespace mpc::lcdgui::screens
