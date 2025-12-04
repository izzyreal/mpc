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

        // May only be invoked from the audio thread! Circumvents the
        // thread-safe message queue.
        // The current implementation does not support "play from start", but
        // that's only because the current intended use case (host sync) does
        // not need it. There's no inherent technical limitation that makes
        // "from start" impossible, so, if necessary, we could at a later point
        // add "from start" support.
        void playImmediately() const;

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

        double getMasterTempo() const;

        void setShouldWaitForMidiClockLock(bool) const;

        bool shouldWaitForMidiClockLock() const;

        void setPositionTicksToReturnToWhenReleasingRec(Tick) const;

    private:
        Sequencer &sequencer;

        bool punchEnabled = false;
        int autoPunchMode = 0;
        int punchInTime = 0;
        int punchOutTime = 0;

        bool isPlayPossible(bool fromStart) const;
    };
} // namespace mpc::sequencer
