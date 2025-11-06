#pragma once

#include "EventAfterNFrames.hpp"

#include <sequencer/LegacyClock.hpp>

#include <memory>
#include <functional>
#include <atomic>

namespace moodycamel
{
    struct ConcurrentQueueDefaultTraits;
    template <typename T, typename Traits> class ConcurrentQueue;
} // namespace moodycamel

namespace mpc::lcdgui
{
    class Screens;
}

namespace mpc::sequencer
{

    class Sequencer;

    class Sequence;

    class MidiClockOutput
    {
    public:
        explicit MidiClockOutput(
            Sequencer *,
            const std::function<std::shared_ptr<lcdgui::Screens>()> &,
            const std::function<bool()> &isBouncing);

        void processTempoChange();
        void processSampleRateChange();
        void processFrame(bool isRunningAtStartOfBuffer, int frameIndex) const;

        void enqueueMidiSyncStart1msBeforeNextClock() const;

        void setSampleRate(unsigned int sampleRate);

        void sendMidiSyncMsg(unsigned char status) const;

        bool isLastProcessedFrameMidiClockLock() const;

    private:
        std::atomic<bool> running{false};
        std::atomic_int32_t requestedSampleRate{44100};
        bool lastProcessedFrameIsMidiClockLock = false;
        LegacyClock clock;
        Sequencer *sequencer;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        std::function<bool()> isBouncing;

        // Has to be called exactly once for each frameIndex
        void processEventsAfterNFrames();

        unsigned char midiClockTickCounter = 0;
        bool wasRunning = false;
        // std::shared_ptr<mpc::engine::midi::ShortMessage>
        // midiSyncStartStopContinueMsg;

        std::shared_ptr<moodycamel::ConcurrentQueue<
            EventAfterNFrames, moodycamel::ConcurrentQueueDefaultTraits>>
            eventQueue;
        std::vector<EventAfterNFrames> tempEventQueue;

        void sendMidiClockMsg(int frameIndex) const;
        void enqueueEventAfterNFrames(const std::function<void()> &event,
                                      unsigned long nFrames) const;

        // std::shared_ptr<mpc::engine::midi::ShortMessage> msg;
    };
} // namespace mpc::sequencer
