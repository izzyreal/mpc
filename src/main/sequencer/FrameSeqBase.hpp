#pragma once

#include "FrameSeqBase.hpp"
#include <sequencer/Clock.hpp>

#include <memory>
#include <utility>
#include <vector>
#include <functional>
#include <atomic>

struct EventAfterNFrames
{
    std::atomic<bool> occupied = ATOMIC_VAR_INIT(false);
    std::function<void(unsigned int)> f = [](unsigned int frameIndex) {};
    unsigned long nFrames = 0;
    unsigned long frameCounter = -1;

    void init(const unsigned long &newNFrames, std::function<void(unsigned int)> callback)
    {
        occupied.store(true);
        nFrames = newNFrames;
        f = std::move(callback);
    }

    void reset()
    {
        f = [](unsigned int frameIndex) {};
        nFrames = 0;
        frameCounter = -1;
        occupied.store(false);
    }
};

namespace ctoot::midi::core { class ShortMessage; }

namespace mpc { class Mpc; }

namespace mpc::lcdgui::screens::window {
    class CountMetronomeScreen;

    class TimingCorrectScreen;
}

namespace mpc::lcdgui::screens {
    class SequencerScreen;

    class UserScreen;

    class SyncScreen;

    class PunchScreen;

    class SongScreen;
}

namespace mpc::sequencer {

    class Sequencer;

    class Sequence;

    class FrameSeqBase
    {
    protected:
        explicit FrameSeqBase(mpc::Mpc &mpc);

        std::atomic<bool> running{false};
        std::atomic_int32_t requestedSampleRate{44100};
        Clock clock;
        std::shared_ptr<Sequencer> sequencer;
        bool metronome = false;

        // Offset of current tick within current buffer
        unsigned short tickFrameOffset = 0;

        unsigned long long continuousTickCounter = 0;

        unsigned long long sequencerPlayTickCounter = 0;

        void updateTimeDisplay();

        void processTempoChange();

        void processSampleRateChange();

        // Has to be called exactly once for each frameIndex
        void processEventsAfterNFrames(int frameIndex);

        // Has to be called exactly once for each tick. If it returns true,
        // the sequencer is now actually playing.
        bool processTransport(bool isRunningAtStartOfBuffer, int frameIndex);

        void triggerClickIfNeeded();

        void displayPunchRects();

        void stopCountingInIfRequired();

        std::shared_ptr<mpc::sequencer::Sequence> switchToNextSequence();

        bool processSongMode();

        bool processSeqLoopEnabled();

        void processSeqLoopDisabled();

        void processNoteRepeat();

    private:
        std::shared_ptr<mpc::lcdgui::screens::window::CountMetronomeScreen> countMetronomeScreen;
        std::shared_ptr<mpc::lcdgui::screens::window::TimingCorrectScreen> timingCorrectScreen;
        std::shared_ptr<mpc::lcdgui::screens::SequencerScreen> sequencerScreen;
        std::shared_ptr<mpc::lcdgui::screens::UserScreen> userScreen;
        std::shared_ptr<mpc::lcdgui::screens::SyncScreen> syncScreen;
        std::shared_ptr<mpc::lcdgui::screens::PunchScreen> punchScreen;
        std::shared_ptr<mpc::lcdgui::screens::SongScreen> songScreen;

        mpc::Mpc &mpc;
        unsigned char midiClockTickCounter = 0;
        bool sequencerShouldStartPlayingOnNextLock = false;
        bool wasRunning = false;
        std::shared_ptr<ctoot::midi::core::ShortMessage> midiSyncStartStopContinueMsg;
        std::vector<EventAfterNFrames> eventsAfterNFrames = std::vector<EventAfterNFrames>(100);

        void move(int newTickPos);

        void stopSequencer();

        void sendMidiClockMsg();

        std::shared_ptr<ctoot::midi::core::ShortMessage> msg;

    public:
        void enqueueMidiSyncStart1msBeforeNextClock();

        void setSampleRate(unsigned int sampleRate);

        void sendMidiSyncMsg(unsigned char status, unsigned int frameIndex);

        void start();

        void startMetronome();

        unsigned short getEventFrameOffset() const;

        void stop();

        bool isRunning();

        unsigned int getTickPosition() const;

        void enqueueEventAfterNFrames(const std::function<void(unsigned int)> &event, unsigned long nFrames);

    };
}
