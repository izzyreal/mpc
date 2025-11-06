#pragma once

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
        int note0 = 34;
        int note1 = 127;

    public:
        void setTime0(int newTime0);
        void setTime1(int newTime1);

        virtual void setNote0(int note);
        void setNote1(int note);

    protected:
        virtual void displayNotes() = 0;
        virtual void displayDrumNotes()
        {
            displayNotes();
        }
        virtual void displayTime() = 0;

    public:
        bool checkAllTimes(Mpc &mpc, int notch,
                           sequencer::Sequence *sequence = nullptr);
        bool checkAllTimesAndNotes(Mpc &mpc, int notch,
                                   sequencer::Sequence *sequence = nullptr,
                                   sequencer::Track *track = nullptr);
    };
} // namespace mpc::lcdgui::screens
