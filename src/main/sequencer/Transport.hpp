#pragma once

#include <cstdint>

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
        bool isCountingIn() const;
        void setCountingIn(bool b);
        void move(double positionQuarterNotes);
        void moveWithinSong(double positionQuarterNotes);
        void setPosition(double positionQuarterNotes);
        void setPositionWithinSong(double positionQuarterNotes);
        void bumpPositionByTicks(uint8_t ticks);
        void resetCountInPositions();

        bool isPlaying() const;
        bool isRecording() const;
        bool isOverdubbing() const;
        bool isRecordingOrOverdubbing() const;

        void setAutoPunchMode(int mode);
        void setPunchEnabled(bool enabled);
        void setPunchInTime(int time);
        void setPunchOutTime(int time);

        void playMetronomeTrack();
        void stopMetronomeTrack();

        int getTickPosition() const;
        double getPlayStartPositionQuarterNotes() const;
        bool isPunchEnabled() const;
        int getAutoPunchMode() const;
        int getPunchInTime() const;
        int getPunchOutTime() const;
        int64_t getCountInStartPos() const;
        int64_t getCountInEndPos() const;

    private:
        Sequencer &sequencer;

        bool playing = false;
        bool recording = false;
        bool overdubbing = false;
        bool countingIn = false;
        bool endOfSong = false;
        bool punchEnabled = false;
        int autoPunchMode = 0;
        int punchInTime = 0;
        int punchOutTime = 0;
        int countInStartPos = -1;
        int countInEndPos = -1;
        int playedStepRepetitions = 0;
    };
} // namespace mpc::sequencer
