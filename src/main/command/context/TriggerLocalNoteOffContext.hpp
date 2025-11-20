#pragma once

#include "performance/PerformanceEventSource.hpp"
#include "sequencer/EventState.hpp"

#include <memory>
#include <functional>
#include <cstdint>
#include <optional>

namespace mpc::performance
{
    class PerformanceManager;
} // namespace mpc::performance

namespace mpc::sampler
{
    class Program;
}

namespace mpc::sequencer
{
    class Bus;
    class Track;
    class DrumBus;
} // namespace mpc::sequencer

namespace mpc::audiomidi
{
    class EventHandler;
}

namespace mpc::command::context
{
    struct TriggerLocalNoteOffContext
    {
        performance::PerformanceEventSource source;
        std::shared_ptr<performance::PerformanceManager> performanceManager;
        std::shared_ptr<sequencer::Bus> bus;
        std::shared_ptr<sampler::Program> program;
        std::optional<int> programPadIndex;
        const bool currentScreenIsSamplerScreen;
        NoteNumber noteNumber;
        std::shared_ptr<audiomidi::EventHandler> eventHandler;
        std::optional<sequencer::EventState> recordOnEvent = std::nullopt;
        const bool sequencerIsRecordingOrOverdubbing;
        const bool isErasePressed;
        sequencer::Track *track;
        const bool isStepRecording;
        const uint64_t metronomeOnlyTickPosition;
        const bool isRecMainWithoutPlaying;
        const int64_t sequencerTickPosition;
        const int tcValuePercentage;
        const int noteValueLengthInTicks;
        const bool isDurationOfRecordedNotesTcValue;
        const bool isAutoStepIncrementEnabled;
        const int currentBarIndex;
        const int swing;
        std::function<int()> sequencerGetActiveSequenceLastTick;
        std::function<void(double)> sequencerMoveToQuarterNotePosition;
        std::function<void()> sequencerStopMetronomeTrack;
    };
} // namespace mpc::command::context
