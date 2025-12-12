#pragma once

#include "performance/PerformanceEventSource.hpp"
#include "sequencer/EventData.hpp"

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
    class Sequencer;
    class SequencerStateManager;
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
        performance::PerformanceManager *performanceManager;
        sequencer::SequencerStateManager *sequencerStateManager;
        sequencer::Bus *bus;
        sampler::Program *program;
        std::optional<int> programPadIndex;
        const bool currentScreenIsSamplerScreen;
        NoteNumber noteNumber;
        audiomidi::EventHandler *eventHandler;
        sequencer::EventData *recordOnEvent = nullptr;
        const bool sequencerIsRecordingOrOverdubbing;
        const bool isErasePressed;
        sequencer::Track *track;
        const bool isStepRecording;
        const Tick metronomeOnlyPositionTicks;
        const bool isRecMainWithoutPlaying;
        const Tick sequencerPositionTicks;
        const int tcValuePercentage;
        const int noteValueLengthInTicks;
        const bool isDurationOfRecordedNotesTcValue;
        const bool isAutoStepIncrementEnabled;
        const int currentBarIndex;
        const int swing;
        sequencer::Sequencer *sequencer;
        Tick positionTicks;
    };
} // namespace mpc::command::context
