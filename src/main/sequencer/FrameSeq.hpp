#pragma once

#include "EventAfterNFrames.hpp"

#include "engine/audio/server/AudioClient.hpp"

#include <memory>
#include <vector>
#include <functional>
#include <atomic>

namespace moodycamel
{
    struct ConcurrentQueueDefaultTraits;
    template <typename T, typename Traits> class ConcurrentQueue;
} // namespace moodycamel

namespace mpc::eventregistry
{
    class EventRegistry;
}

namespace mpc::hardware
{
    class Slider;
}

namespace mpc::engine::audio::mixer
{
    class AudioMixer;
}

namespace mpc::engine
{
    class Voice;
    class MixerInterconnection;
} // namespace mpc::engine

namespace mpc::lcdgui::screens::window
{
    class CountMetronomeScreen;

    class TimingCorrectScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::lcdgui
{
    class LayeredScreen;
    class Screens;
} // namespace mpc::lcdgui

namespace mpc::lcdgui::screens
{
    class SequencerScreen;
    class UserScreen;
    class PunchScreen;
    class SongScreen;
    class SyncScreen;
} // namespace mpc::lcdgui::screens

namespace mpc::sampler
{
    class Sampler;
}

namespace mpc::engine
{
    class Voice;
    class MixerInterconnection;
} // namespace mpc::engine

namespace mpc::lcdgui::screens::window
{
    class Assign16LevelsScreen;
}

namespace mpc::lcdgui::screens
{
    class MixerSetupScreen;
}

namespace mpc::sequencer
{
    class Sequencer;
    class Clock;
    class Sequence;
    class MidiClockOutput;

    class FrameSeq : public mpc::engine::audio::server::AudioClient
    {
    public:
        explicit FrameSeq(
            std::shared_ptr<eventregistry::EventRegistry>,
            std::shared_ptr<Sequencer>, std::shared_ptr<Clock>,
            std::shared_ptr<lcdgui::LayeredScreen>,
            std::function<bool()> isBouncing,
            std::function<int()> getSampleRate,
            std::function<bool()> isRecMainWithoutPlaying,
            std::function<void(int velo, int frameOffset)> playMetronome,
            std::shared_ptr<lcdgui::Screens>,
            std::function<bool()> isNoteRepeatLockedOrPressed,
            std::shared_ptr<sampler::Sampler>,
            std::shared_ptr<engine::audio::mixer::AudioMixer>,
            std::function<bool()> isFullLevelEnabled,
            std::function<bool()> isSixteenLevelsEnabled,
            std::shared_ptr<hardware::Slider> hardwareSlider,
            std::vector<std::shared_ptr<engine::Voice>> *,
            std::vector<engine::MixerInterconnection *> &);

        void work(int nFrames) override;

        void setSampleRate(unsigned int sampleRate);

        void start(bool metronomeOnly = false);

        void startMetronome();

        unsigned short getEventFrameOffset() const;

        void stop();

        bool isRunning();

        void enqueueEventAfterNFrames(const std::function<void()> &event,
                                      unsigned long nFrames);

        uint64_t getMetronomeOnlyTickPosition();

    private:
        std::shared_ptr<eventregistry::EventRegistry> eventRegistry;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::shared_ptr<Sequencer> sequencer;
        std::shared_ptr<Clock> clock;
        std::function<bool()> isBouncing;
        std::function<int()> getSampleRate;
        std::function<bool()> isRecMainWithoutPlaying;
        std::function<void(int velo, int frameOffset)> triggerMetronome;
        std::function<bool()> isNoteRepeatLockedOrPressed;

        // Required for note repeat (RepeatPad)
        std::shared_ptr<sampler::Sampler> sampler;
        std::shared_ptr<engine::audio::mixer::AudioMixer> audioMixer;
        std::function<bool()> isFullLevelEnabled;
        std::function<bool()> isSixteenLevelsEnabled;
        std::shared_ptr<lcdgui::screens::window::Assign16LevelsScreen>
            assign16LevelsScreen;
        std::shared_ptr<lcdgui::screens::MixerSetupScreen> mixerSetupScreen;
        std::shared_ptr<hardware::Slider> hardwareSlider;
        std::vector<std::shared_ptr<engine::Voice>> *voices;
        std::vector<engine::MixerInterconnection *> &mixerInterconnections;

        std::atomic<bool> sequencerIsRunning{false};
        double previousTempo = 0.0;
        bool shouldWaitForMidiClockLock = false;
        std::atomic_int32_t requestedSampleRate{44100};
        bool metronomeOnly = false;
        std::shared_ptr<lcdgui::screens::SyncScreen> syncScreen;
        std::shared_ptr<MidiClockOutput> midiClockOutput;

        // Offset of current tick within current buffer
        unsigned short tickFrameOffset = 0;

        // Has to be called exactly once for each frameIndex
        void processEventsAfterNFrames();

        void triggerClickIfNeeded();

        void displayPunchRects();

        void stopCountingInIfRequired();

        std::shared_ptr<sequencer::Sequence> switchToNextSequence();

        bool processSongMode();

        bool processSeqLoopEnabled();

        bool processSeqLoopDisabled();

        void processNoteRepeat();

    private:
        std::shared_ptr<lcdgui::screens::window::CountMetronomeScreen>
            countMetronomeScreen;
        std::shared_ptr<lcdgui::screens::window::TimingCorrectScreen>
            timingCorrectScreen;
        std::shared_ptr<lcdgui::screens::SequencerScreen> sequencerScreen;
        std::shared_ptr<lcdgui::screens::UserScreen> userScreen;
        std::shared_ptr<lcdgui::screens::PunchScreen> punchScreen;
        std::shared_ptr<lcdgui::screens::SongScreen> songScreen;

        std::shared_ptr<moodycamel::ConcurrentQueue<
            EventAfterNFrames, moodycamel::ConcurrentQueueDefaultTraits>>
            eventQueue;
        std::vector<EventAfterNFrames> tempEventQueue;

        void move(int newTickPos);

        void stopSequencer();

        uint64_t metronomeOnlyTickPosition = 0;
    };
} // namespace mpc::sequencer
