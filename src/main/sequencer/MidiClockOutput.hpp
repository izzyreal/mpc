#pragma once

#include "EventAfterNFrames.hpp"

#include <engine/audio/server/AudioClient.hpp>

#include <sequencer/LegacyClock.hpp>
#include "lcdgui/screens/SyncScreen.hpp"

#include <memory>
#include <utility>
#include <vector>
#include <functional>
#include <atomic>

namespace mpc::engine::midi
{
    class ShortMessage;
}

namespace mpc
{
    class Mpc;
}

namespace mpc::lcdgui::screens::window
{
    class CountMetronomeScreen;

    class TimingCorrectScreen;
} // namespace mpc::lcdgui::screens::window

namespace mpc::sequencer
{

    class Sequencer;

    class Sequence;

    class MidiClockOutput
    {
    private:
        std::atomic<bool> running{false};
        std::atomic_int32_t requestedSampleRate{44100};
        bool lastProcessedFrameIsMidiClockLock = false;
        LegacyClock clock;
        std::shared_ptr<Sequencer> sequencer;
        std::shared_ptr<mpc::lcdgui::screens::SyncScreen> syncScreen;

        // Has to be called exactly once for each frameIndex
        void processEventsAfterNFrames(int frameIndex);

        mpc::Mpc &mpc;
        unsigned char midiClockTickCounter = 0;
        bool wasRunning = false;
        std::shared_ptr<mpc::engine::midi::ShortMessage>
            midiSyncStartStopContinueMsg;
        std::vector<EventAfterNFrames> eventsAfterNFrames =
            std::vector<EventAfterNFrames>(50);

        void sendMidiClockMsg(int frameIndex);
        void enqueueEventAfterNFrames(const std::function<void()> &event,
                                      unsigned long nFrames);

        std::shared_ptr<mpc::engine::midi::ShortMessage> msg;

    public:
        explicit MidiClockOutput(mpc::Mpc &mpc);

        void processTempoChange();
        void processSampleRateChange();
        void processFrame(bool isRunningAtStartOfBuffer, int frameIndex);

        void enqueueMidiSyncStart1msBeforeNextClock();

        void setSampleRate(unsigned int sampleRate);

        void sendMidiSyncMsg(unsigned char status);

        bool isLastProcessedFrameMidiClockLock();
    };
} // namespace mpc::sequencer
