#pragma once

#include "engine/EventAfterNFrames.hpp"

#include "sequencer/LegacyClock.hpp"
#include "client/event/ClientMidiEvent.hpp"

#include <memory>
#include <functional>
#include <atomic>

namespace moodycamel
{
    struct ConcurrentQueueDefaultTraits;
    template <typename T, typename Traits> class ConcurrentQueue;
} // namespace moodycamel


namespace mpc::audiomidi
{
    class MidiOutput;
}

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
        const std::function<int()> &getSampleRate,
            const std::function<std::shared_ptr<audiomidi::MidiOutput>()>
                &getMidiOutput,
            Sequencer *,
            const std::function<std::shared_ptr<lcdgui::Screens>()> &,
            const std::function<bool()> &isBouncing);

        void processTempoChange();
        void processSampleRateChange();
        void processFrame(bool isRunningAtStartOfBuffer, int frameIndex);

        void enqueueMidiSyncStart1msBeforeNextClock() const;

        void sendMidiSyncMsg(client::event::ClientMidiEvent::MessageType, int sampleNumber) const;

        bool isLastProcessedFrameMidiClockLock() const;

    private:
        const std::function<int()> getSampleRate;
        std::function<std::shared_ptr<audiomidi::MidiOutput>()> getMidiOutput;
        std::atomic<bool> running{false};
        std::atomic_int32_t requestedSampleRate{44100};
        bool lastProcessedFrameIsMidiClockLock = false;
        LegacyClock clock;
        Sequencer *sequencer;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        std::function<bool()> isBouncing;

        // Has to be called exactly once for each frameIndex
        void processEventsAfterNFrames(int);

        unsigned char midiClockTickCounter = 0;
        bool wasRunning = false;

        std::shared_ptr<moodycamel::ConcurrentQueue<
            engine::EventAfterNFrames,
            moodycamel::ConcurrentQueueDefaultTraits>>
            eventQueue;
        std::vector<engine::EventAfterNFrames> tempEventQueue;

        void sendMidiClockMsg(int frameIndex) const;
        void enqueueEventAfterNFrames(const std::function<void(int)> &event,
                                      unsigned long nFrames) const;
    };
} // namespace mpc::sequencer
