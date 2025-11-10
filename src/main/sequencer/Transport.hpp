#pragma once

#include <cstdint>

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
        enum StopMode
        {
            AT_START_OF_BUFFER,
            AT_START_OF_TICK
        };

        explicit Transport(Sequencer &owner);

        void play();
        void play(bool fromStart);
        void playFromStart();
        void rec();
        void recFromStart();
        void overdub();
        void overdubFromStart();
        void switchRecordToOverdub();
        void stop();
        void stop(const StopMode);
        void setRecording(bool b);
        void setOverdubbing(bool b);
        void setPosition(double positionQuarterNotes,
                         bool shouldSyncTrackEventIndicesToNewPosition = true,
                         bool shouldSetPlayStartPosition = true);
        void
        setPositionWithinSong(double positionQuarterNotes,
                              bool shouldSyncTrackEventIndicesToNewPosition = true,
                              bool shouldSetPlayStartPosition = true);
        void bumpPositionByTicks(uint8_t ticks);
        bool isPlaying() const;
        bool isRecording() const;
        bool isOverdubbing() const;
        bool isRecordingOrOverdubbing() const;

        void setTempo(double newTempo);
        double getTempo();
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
        double getPlayStartPositionQuarterNotes() const;
        int getCurrentBarIndex();
        int getCurrentBeatIndex();
        int getCurrentClockNumber();
        void setBar(int i);
        void setBeat(int i);
        void setClock(int i);

        bool isCountEnabled() const;
        void setCountEnabled(bool b);
        bool isCountingIn() const;
        void setCountingIn(bool b);
        void resetCountInPositions();
        int64_t getCountInStartPos() const;
        int64_t getCountInEndPos() const;

        bool isPunchEnabled() const;
        int getAutoPunchMode() const;
        int getPunchInTime() const;
        int getPunchOutTime() const;

        void setEndOfSong(bool);
        int getPlayedStepRepetitions() const;
        void incrementPlayedStepRepetitions();
        void resetPlayedStepRepetitions();

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
