#pragma once

#include <functional>
#include <cstdint>

namespace mpc::sequencer {
    class NoteOnEventPlayOnly;
    class NoteOnEvent;
    class Track;
}

namespace mpc::audiomidi {
    class EventHandler;
}

namespace mpc::controls {

struct PadReleaseContext {
    const int padIndexWithBank;
    std::function<void()> finishBasicVoiceIfSoundIsLooping;
    const bool currentScreenIsSoundScreen;
    const bool currentScreenIsSamplerScreen;
    std::function<void(int)> registerProgramPadRelease;
    std::shared_ptr<mpc::sequencer::NoteOnEventPlayOnly> playNoteEvent;
    const int drumScreenSelectedDrum;
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
}
