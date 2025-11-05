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

    class FrameSeq : public mpc::engine::audio::server::AudioClient
    {
    public:
        explicit FrameSeq(
            std::shared_ptr<eventregistry::EventRegistry>, Sequencer *,
            std::shared_ptr<Clock>, std::shared_ptr<lcdgui::LayeredScreen>,
            std::function<bool()> isBouncing,
            std::function<int()> getSampleRate,
            std::function<bool()> isRecMainWithoutPlaying,
            std::function<void(int velo, int frameOffset)> playMetronome,
            std::function<std::shared_ptr<lcdgui::Screens>()>,
            std::function<bool()> isNoteRepeatLockedOrPressed,
            std::shared_ptr<sampler::Sampler>,
            std::function<std::shared_ptr<engine::audio::mixer::AudioMixer>()>,
            std::function<bool()> isFullLevelEnabled,
            std::function<bool()> isSixteenLevelsEnabled,
            std::shared_ptr<hardware::Slider> hardwareSlider,
            std::vector<std::shared_ptr<engine::Voice>> *,
            std::function<
                std::vector<mpc::engine::MixerInterconnection *> &()>);

        void work(int nFrames) override;

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
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        Sequencer *sequencer;
        std::shared_ptr<Clock> clock;
        std::function<bool()> isBouncing;
        std::function<int()> getSampleRate;
        std::function<bool()> isRecMainWithoutPlaying;
        std::function<void(int velo, int frameOffset)> triggerMetronome;
        std::function<bool()> isNoteRepeatLockedOrPressed;

        // Required for note repeat (RepeatPad)
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

        void triggerClickIfNeeded();

        void displayPunchRects();

        void stopCountingInIfRequired();

        std::shared_ptr<sequencer::Sequence> switchToNextSequence();

        bool processSongMode();

        bool processSeqLoopEnabled();

        bool processSeqLoopDisabled();

        void processNoteRepeat();

    private:
        std::shared_ptr<moodycamel::ConcurrentQueue<
            EventAfterNFrames, moodycamel::ConcurrentQueueDefaultTraits>>
            eventQueue;
        std::vector<EventAfterNFrames> tempEventQueue;

        void move(int newTickPos);

        void stopSequencer();

        uint64_t metronomeOnlyTickPosition = 0;
    };
} // namespace mpc::sequencer
