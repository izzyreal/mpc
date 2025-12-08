#pragma once

#include "sequencer/LegacyClock.hpp"
#include "client/event/ClientMidiEvent.hpp"

#include <memory>
#include <functional>
#include <atomic>

namespace mpc::engine
{
    class EngineHost;
}
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
            engine::EngineHost *, const std::function<int()> &getSampleRate,
            const std::function<std::shared_ptr<audiomidi::MidiOutput>()>
                &getMidiOutput,
            Sequencer *,
            const std::function<std::shared_ptr<lcdgui::Screens>()> &,
            const std::function<bool()> &isBouncing);

        void processTempoChange();
        void processSampleRateChange();
        void processFrame(bool isRunningAtStartOfBuffer, int frameIndex,
                          int tickCountAtThisFrame);

        void enqueueMidiSyncStart1msBeforeNextClock() const;

        void sendMidiSyncMsg(client::event::ClientMidiEvent::MessageType,
                             int sampleNumber) const;

        bool isLastProcessedFrameMidiClockLock() const;

    private:
        engine::EngineHost *engineHost;
        const std::function<int()> getSampleRate;
        std::function<std::shared_ptr<audiomidi::MidiOutput>()> getMidiOutput;
        std::atomic<bool> running{false};
        std::atomic_int32_t requestedSampleRate{44100};
        bool lastProcessedFrameIsMidiClockLock = false;
        LegacyClock clock;
        Sequencer *sequencer;
        std::function<std::shared_ptr<lcdgui::Screens>()> getScreens;
        std::function<bool()> isBouncing;

        unsigned char midiClockTickCounter = 0;
        bool wasRunning = false;

        void sendMidiClockMsg(int frameIndex) const;
    };
} // namespace mpc::sequencer
