#pragma once

#include "FloatTypes.hpp"
#include "IntTypes.hpp"

namespace mpc::lcdgui
{
    class Screens;
}

namespace mpc::sequencer
{
    class Sequencer;

    class Transport
    {
    public:
        explicit Transport(Sequencer &owner);

        void play(bool fromStart = false) const;
        void rec() const;
        void recFromStart() const;
        void overdub() const;
        void overdubFromStart() const;
        void switchRecordToOverdub() const;
        void stop() const;
        void setRecording(bool b) const;
        void setOverdubbing(bool b) const;

        PositionQuarterNotes
            getWrappedPositionInSequence(PositionQuarterNotes) const;

        PositionQuarterNotes
            getWrappedPositionInSong(PositionQuarterNotes) const;

        void moveSongToStepThatContainsPosition(PositionQuarterNotes) const;

        void setPosition(PositionQuarterNotes) const;

        bool isPlaying() const;
        bool isRecording() const;
        bool isOverdubbing() const;
        bool isRecordingOrOverdubbing() const;

        void setTempo(double newTempo) const;
        double getTempo() const;
        bool isTempoSourceSequence() const;
        void setTempoSourceIsSequence(bool b) const;

        void setAutoPunchMode(int mode);
        void setPunchEnabled(bool enabled);
        void setPunchInTime(int time);
        void setPunchOutTime(int time);

        bool isMetronomeOnlyEnabled() const;
        void playMetronomeOnly() const;
        void stopMetronomeOnly() const;

        int getTickPosition() const;
        Tick getTickPositionGuiPresentation() const;
        double getPositionQuarterNotes() const;
        double getPlayStartPositionQuarterNotes() const;
        int getCurrentBarIndex() const;
        int getCurrentBeatIndex() const;
        int getCurrentClockNumber() const;
        void setBarBeatClock(int bar, int beat, int clock) const;
        void setBar(int i) const;
        void setBeat(int i) const;
        void setClock(int i) const;

        bool isCountEnabled() const;
        void setCountEnabled(bool b) const;
        bool isCountingIn() const;
        void setCountingIn(bool b) const;
        void resetCountInPositions() const;
        Tick getCountInStartPosTicks() const;
        Tick getCountInEndPosTicks() const;

        bool isPunchEnabled() const;
        int getAutoPunchMode() const;
        int getPunchInTime() const;
        int getPunchOutTime() const;

        void setEndOfSong(bool) const;
        int getPlayedStepRepetitions() const;
        void incrementPlayedStepRepetitions() const;
        void resetPlayedStepRepetitions() const;

        void bumpPositionByTicks(Tick) const;

        void setMasterTempo(double) const;

    private:
        Sequencer &sequencer;

        bool punchEnabled = false;
        int autoPunchMode = 0;
        int punchInTime = 0;
        int punchOutTime = 0;
    };
} // namespace mpc::sequencer
