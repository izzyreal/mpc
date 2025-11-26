#pragma once

#include "FloatTypes.hpp"

#include <cstdint>
#include <memory>
#include <functional>

namespace mpc::lcdgui
{
    class Screens;
}

namespace mpc::engine
{
    class SequencerPlaybackEngine;
}

namespace mpc::sequencer
{
    class Sequencer;

    class Transport
    {
    public:
        explicit Transport(
            Sequencer &owner,
            const std::function<
                std::shared_ptr<engine::SequencerPlaybackEngine>()> &);

        void play(bool fromStart = false) const;
        void rec();
        void recFromStart();
        void overdub();
        void overdubFromStart();
        void switchRecordToOverdub();
        void stop() const;
        void setRecording(bool b);
        void setOverdubbing(bool b);

        PositionQuarterNotes getWrappedPositionInSequence(PositionQuarterNotes) const;

        PositionQuarterNotes getWrappedPositionInSong(PositionQuarterNotes) const;

        void moveSongToStepThatContainsPosition(PositionQuarterNotes) const;

        void setPosition(PositionQuarterNotes) const;

        void setPlayStartPosition(PositionQuarterNotes) const;

        void bumpPositionByTicks(uint8_t tickCount) const;

        bool isPlaying() const;
        bool isRecording() const;
        bool isOverdubbing() const;
        bool isRecordingOrOverdubbing() const;

        void setTempo(double newTempo);
        double getTempo() const;
        bool isTempoSourceSequenceEnabled() const;
        void setTempoSourceSequence(bool b);

        void setAutoPunchMode(int mode);
        void setPunchEnabled(bool enabled);
        void setPunchInTime(int time);
        void setPunchOutTime(int time);

        bool isMetronomeOnlyEnabled() const;
        void playMetronomeTrack();
        void stopMetronomeTrack();

        int getTickPosition() const;
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
        void setCountEnabled(bool b);
        bool isCountingIn() const;
        void setCountingIn(bool b);
        void resetCountInPositions();
        int64_t getCountInStartPosTicks() const;
        int64_t getCountInEndPosTicks() const;
        void setCountInStartPosTicks(int64_t);
        void setCountInEndPosTicks(int64_t);

        bool isPunchEnabled() const;
        int getAutoPunchMode() const;
        int getPunchInTime() const;
        int getPunchOutTime() const;

        void setEndOfSong(bool);
        int getPlayedStepRepetitions() const;
        void incrementPlayedStepRepetitions();
        void resetPlayedStepRepetitions();

        const std::function<std::shared_ptr<engine::SequencerPlaybackEngine>()>
            getSequencerPlaybackEngine;

    private:
        Sequencer &sequencer;

        bool playing = false;
        bool recording = false;
        bool overdubbing = false;
        bool countingIn = false;
        bool metronomeOnlyEnabled = false;
        bool endOfSong = false;
        bool punchEnabled = false;
        bool countEnabled = true;
        int autoPunchMode = 0;
        int punchInTime = 0;
        int punchOutTime = 0;
        int countInStartPos = -1;
        int countInEndPos = -1;
        int playedStepRepetitions = 0;

        double tempo = 120.0;
        bool tempoSourceSequenceEnabled = true;
    };
} // namespace mpc::sequencer
