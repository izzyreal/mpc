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

namespace mpc::lcdgui
{
    class LayeredScreen;
    class Screens;
} // namespace mpc::lcdgui

namespace mpc::sampler
{
    class Sampler;
}

namespace mpc::engine
{
    class Voice;
    class MixerInterconnection;
} // namespace mpc::engine

namespace mpc::sequencer
{
    class Sequencer;
    class Clock;
    class Sequence;
    class MidiClockOutput;

    class FrameSeq final : public engine::audio::server::AudioClient
    {
        using EventQueue = moodycamel::ConcurrentQueue<
            EventAfterNFrames, moodycamel::ConcurrentQueueDefaultTraits>;

    public:
        explicit FrameSeq(
            const std::shared_ptr<eventregistry::EventRegistry> &, Sequencer *,
            const std::shared_ptr<Clock> &,
            const std::shared_ptr<lcdgui::LayeredScreen> &,
            std::function<bool()> isBouncing,
            const std::function<int()> &getSampleRate,
            const std::function<bool()> &isRecMainWithoutPlaying,
            const std::function<void(int velo, int frameOffset)> &playMetronome,
            std::function<std::shared_ptr<lcdgui::Screens>()>,
            const std::function<bool()> &isNoteRepeatLockedOrPressed,
            const std::shared_ptr<sampler::Sampler> &,
            const std::function<
                std::shared_ptr<engine::audio::mixer::AudioMixer>()> &,
            const std::function<bool()> &isFullLevelEnabled,
            const std::function<bool()> &isSixteenLevelsEnabled,
            const std::shared_ptr<hardware::Slider> &hardwareSlider,
            std::vector<std::shared_ptr<engine::Voice>> *,
            const std::function<std::vector<engine::MixerInterconnection *> &()>
                &);

        void work(int nFrames) override;

        void start(bool metronomeOnly = false);

        void startMetronome();

        unsigned short getEventFrameOffset() const;

        void stop();

        bool isRunning() const;

        void enqueueEventAfterNFrames(const std::function<void()> &event,
                                      unsigned long nFrames) const;

        uint64_t getMetronomeOnlyTickPosition() const;

    private:
        std::shared_ptr<EventQueue> eventQueue;
        std::vector<EventAfterNFrames> tempEventQueue;

        std::shared_ptr<eventregistry::EventRegistry> eventRegistry;
        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        Sequencer *sequencer;
        std::shared_ptr<Clock> clock;
        std::function<bool()> isBouncing;
        std::function<int()> getSampleRate;
        std::function<bool()> isRecMainWithoutPlaying;
        std::function<void(int velo, int frameOffset)> triggerMetronome;
        std::function<bool()> isNoteRepeatLockedOrPressed;

        // Required for note repeat (NoteRepeatProcessor)
        std::shared_ptr<sampler::Sampler> sampler;
        std::function<std::shared_ptr<engine::audio::mixer::AudioMixer>()>
            getAudioMixer;
        std::function<bool()> isFullLevelEnabled;
        std::function<bool()> isSixteenLevelsEnabled;
        std::shared_ptr<hardware::Slider> hardwareSlider;
        std::vector<std::shared_ptr<engine::Voice>> *voices;
        std::function<std::vector<engine::MixerInterconnection *> &()>
            getMixerInterconnections;

        std::atomic<bool> sequencerIsRunning{false};
        double previousTempo = 0.0;
        bool shouldWaitForMidiClockLock = false;
        bool metronomeOnly = false;
        std::shared_ptr<MidiClockOutput> midiClockOutput;

        // Offset of current tick within current buffer
        unsigned short tickFrameOffset = 0;

        // Has to be called exactly once for each frameIndex
        void processEventsAfterNFrames();

        void triggerClickIfNeeded() const;

        void displayPunchRects() const;

        void stopCountingInIfRequired() const;

        std::shared_ptr<Sequence> switchToNextSequence() const;

        bool processSongMode() const;

        bool processSeqLoopEnabled() const;

        bool processSeqLoopDisabled() const;

        void processNoteRepeat();

        void move(int newTickPos) const;

        void stopSequencer() const;

        uint64_t metronomeOnlyTickPosition = 0;
    };
} // namespace mpc::sequencer
