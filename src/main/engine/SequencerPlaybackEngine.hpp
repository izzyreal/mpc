#pragma once

#include "engine/EventAfterNFrames.hpp"

#include "engine/audio/server/AudioClient.hpp"
#include "engine/NoteRepeatProcessor.hpp"

#include <memory>
#include <vector>
#include <functional>

namespace moodycamel
{
    struct ConcurrentQueueDefaultTraits;
    template <typename T, typename Traits> class ConcurrentQueue;
} // namespace moodycamel

namespace mpc::sequencer
{
    class Sequencer;
    class Clock;
    class Sequence;
    class MidiClockOutput;
} // namespace mpc::sequencer

namespace mpc::lcdgui
{
    class LayeredScreen;
    class Screens;
} // namespace mpc::lcdgui

namespace mpc::engine
{
    class NoteRepeatProcessor;

    class SequencerPlaybackEngine final : public audio::server::AudioClient
    {
        using EventQueue = moodycamel::ConcurrentQueue<
            EventAfterNFrames, moodycamel::ConcurrentQueueDefaultTraits>;

    public:
        explicit SequencerPlaybackEngine(
            sequencer::Sequencer *, const std::shared_ptr<sequencer::Clock> &,
            const std::shared_ptr<lcdgui::LayeredScreen> &,
            std::function<bool()> isBouncing,
            const std::function<int()> &getSampleRate,
            const std::function<bool()> &isRecMainWithoutPlaying,
            const std::function<void(int velo, int frameOffset)> &playMetronome,
            std::function<std::shared_ptr<lcdgui::Screens>()>,
            const std::function<bool()> &isNoteRepeatLockedOrPressed,
            const std::shared_ptr<NoteRepeatProcessor> &,
            const std::function<bool()> &isAudioServerCurrentlyRunningOffline);

        void work(int nFrames) override;
        uint64_t getMetronomeOnlyTickPosition() const;

        void enqueueEventAfterNFrames(const std::function<void()> &event,
                                      unsigned long nFrames) const;

        const std::function<int()> getSampleRate;

    private:
        std::shared_ptr<EventQueue> eventQueue;
        std::vector<EventAfterNFrames> tempEventQueue;

        std::shared_ptr<lcdgui::LayeredScreen> layeredScreen;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        sequencer::Sequencer *sequencer;
        std::shared_ptr<sequencer::Clock> clock;
        std::function<bool()> isBouncing;
        std::function<bool()> isRecMainWithoutPlaying;
        std::function<void(int velo, int frameOffset)> playMetronome;
        std::function<bool()> isNoteRepeatLockedOrPressed;
        std::shared_ptr<NoteRepeatProcessor> noteRepeatProcessor;
        std::function<bool()> isAudioServerCurrentlyRunningOffline;

        double previousTempo = 0.0;
        bool shouldWaitForMidiClockLock = false;
        std::shared_ptr<sequencer::MidiClockOutput> midiClockOutput;

        // Has to be called exactly once for each frameIndex
        void processEventsAfterNFrames();

        uint64_t metronomeOnlyTickPosition = 0;
    };
} // namespace mpc::engine
