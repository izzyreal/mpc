#pragma once

#include <memory>
#include <functional>
#include <cstdint>

namespace mpc::eventregistry
{
    class EventRegistry;
}

namespace mpc::sampler
{
    class Program;
}

namespace mpc::sequencer
{
    class NoteOnEventPlayOnly;
    class NoteOnEvent;
    class Track;
    class DrumBus;
} // namespace mpc::sequencer

namespace mpc::audiomidi
{
    class EventHandler;
}

namespace mpc::command::context
{
    struct TriggerDrumNoteOffContext
    {
        std::shared_ptr<eventregistry::EventRegistry> eventRegistry;
        std::shared_ptr<sequencer::DrumBus> drumBus;
        std::shared_ptr<sampler::Program> program;
        int programPadIndex;
        std::function<void()> finishBasicVoiceIfSoundIsLooping;
        const bool currentScreenIsSoundScreen;
        const bool currentScreenIsSamplerScreen;
        std::shared_ptr<mpc::sequencer::NoteOnEventPlayOnly> playNoteEvent;
        std::optional<int> drumIndex;
        std::shared_ptr<mpc::audiomidi::EventHandler> eventHandler;
        std::shared_ptr<mpc::sequencer::NoteOnEvent> recordOnEvent;
        const bool sequencerIsRecordingOrOverdubbing;
        const bool isErasePressed;
        const std::shared_ptr<mpc::sequencer::Track> activeTrack;
        const bool isStepRecording;
        const std::function<bool()> isAnyProgramPadRegisteredAsPressed;
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
