#pragma once

#include "eventregistry/PerformanceManager.hpp"
#include <memory>
#include <functional>
#include <cstdint>

namespace mpc::eventregistry
{
    class PerformanceManager;
    class NoteOnEvent;
} // namespace mpc::eventregistry

namespace mpc::sampler
{
    class Program;
}

namespace mpc::sequencer
{
    class Bus;
    class NoteOnEvent;
    class NoteOffEvent;
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
        eventregistry::PerformanceEventSource source;
        std::shared_ptr<eventregistry::PerformanceManager> performanceManager;
        std::shared_ptr<sequencer::Bus> bus;
        std::shared_ptr<sampler::Program> program;
        std::optional<int> programPadIndex;
        const bool currentScreenIsSamplerScreen;
        std::shared_ptr<sequencer::NoteOffEvent> noteOffEvent;
        std::shared_ptr<audiomidi::EventHandler> eventHandler;
        std::shared_ptr<sequencer::NoteOnEvent> recordOnEvent;
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
