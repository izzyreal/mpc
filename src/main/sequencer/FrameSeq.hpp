#pragma once

#include "EventAfterNFrames.hpp"

#include <engine/audio/server/AudioClient.hpp>

#include "Clock.hpp"
#include "MidiClockOutput.hpp"
#include "lcdgui/screens/SyncScreen.hpp"

#include <memory>
#include <utility>
#include <vector>
#include <functional>
#include <atomic>

namespace mpc { class Mpc; }

namespace mpc::lcdgui::screens::window {
    class CountMetronomeScreen;

    class TimingCorrectScreen;
}

namespace mpc::lcdgui::screens {
    class SequencerScreen;

    class UserScreen;

    class PunchScreen;

    class SongScreen;
}

namespace mpc::sequencer {

    class Sequencer;

    class Sequence;

    class FrameSeq : public mpc::engine::audio::server::AudioClient
    {
    private:
        std::atomic<bool> sequencerIsRunning{false};
        bool shouldWaitForMidiClockLock = false;
        std::atomic_int32_t requestedSampleRate{44100};
        Clock clock;
        std::shared_ptr<MidiClockOutput> midiClockOutput;
        std::shared_ptr<Sequencer> sequencer;
        bool metronome = false;
        std::shared_ptr<mpc::lcdgui::screens::SyncScreen> syncScreen;

        // Offset of current tick within current buffer
        unsigned short tickFrameOffset = 0;

        unsigned long long sequencerPlayTickCounter = 0;

        void updateTimeDisplay();

        void processTempoChange();

        void processSampleRateChange();

        // Has to be called exactly once for each frameIndex
        void processEventsAfterNFrames(int frameIndex);

        void triggerClickIfNeeded();

        void displayPunchRects();

        void stopCountingInIfRequired();

        std::shared_ptr<mpc::sequencer::Sequence> switchToNextSequence();

        bool processSongMode();

        bool processSeqLoopEnabled();

        bool processSeqLoopDisabled();

        void processNoteRepeat();

    private:
        std::shared_ptr<mpc::lcdgui::screens::window::CountMetronomeScreen> countMetronomeScreen;
        std::shared_ptr<mpc::lcdgui::screens::window::TimingCorrectScreen> timingCorrectScreen;
        std::shared_ptr<mpc::lcdgui::screens::SequencerScreen> sequencerScreen;
        std::shared_ptr<mpc::lcdgui::screens::UserScreen> userScreen;
        std::shared_ptr<mpc::lcdgui::screens::PunchScreen> punchScreen;
        std::shared_ptr<mpc::lcdgui::screens::SongScreen> songScreen;

        mpc::Mpc &mpc;
        std::vector<EventAfterNFrames> eventsAfterNFrames = std::vector<EventAfterNFrames>(50);

        void move(int newTickPos);

        void stopSequencer();

    public:
        explicit FrameSeq(mpc::Mpc &mpc);

        void work(int nFrames) override;

        void setSampleRate(unsigned int sampleRate);

        void start();

        void startMetronome();

        unsigned short getEventFrameOffset() const;

        void stop();

        bool isRunning();

        unsigned int getTickPosition() const;

        void enqueueEventAfterNFrames(const std::function<void(unsigned int)> &event, unsigned long nFrames);

        void setSequencerPlayTickCounter(unsigned long long);

    };
}
