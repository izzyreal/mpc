#pragma once

#include "EventAfterNFrames.hpp"

#include <engine/audio/server/AudioClient.hpp>

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
        double previousTempo = 0.0;
        bool shouldWaitForMidiClockLock = false;
        std::atomic_int32_t requestedSampleRate{44100};
        std::shared_ptr<MidiClockOutput> midiClockOutput;
        std::shared_ptr<Sequencer> sequencer;
        bool metronomeOnly = false;
        std::shared_ptr<mpc::lcdgui::screens::SyncScreen> syncScreen;

        // Offset of current tick within current buffer
        unsigned short tickFrameOffset = 0;

        void updateTimeDisplay();

        void processTempoChange();

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

        uint64_t metronomeOnlyTickPosition = 0;

    public:
        explicit FrameSeq(mpc::Mpc &mpc);

        void work(int nFrames) override;

        void setSampleRate(unsigned int sampleRate);

        void start(bool metronomeOnly = false);

        void startMetronome();

        unsigned short getEventFrameOffset() const;

        void stop();

        bool isRunning();

        void enqueueEventAfterNFrames(const std::function<void()> &event, unsigned long nFrames);

        uint64_t getMetronomeOnlyTickPosition();

    };
}
