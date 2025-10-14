#include "ReleasePadCommand.h"

#include "audiomidi/EventHandler.hpp"
#include "controls/PadReleaseContext.h"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::command;
using namespace mpc::controls;

ReleasePadCommand::ReleasePadCommand(PadReleaseContext& ctx)
    : ctx(ctx) {}

void ReleasePadCommand::execute()
{
    ctx.finishBasicVoiceIfSoundIsLooping();

    if (ctx.currentScreenIsSoundScreen)
    {
        return;
    }

    if (!ctx.playNoteEvent)
    {
        return;
    }

    const auto noteOff = ctx.playNoteEvent->getNoteOff();
    noteOff->setTick(-1);

    const auto drum = ctx.currentScreenIsSamplerScreen
        ? std::optional<uint8_t>(ctx.drumScreenSelectedDrum)
        : std::optional<uint8_t>();

    ctx.eventHandler->handle(noteOff, ctx.activeTrack.get(), drum);

    ctx.registerProgramPadRelease(ctx.padIndexWithBank);

    if (!ctx.recordOnEvent)
    {
        return;
    }

    if (ctx.sequencerIsRecordingOrOverdubbing && ctx.isErasePressed)
    {
        return;
    }

    if (ctx.sequencerIsRecordingOrOverdubbing)
    {
        ctx.activeTrack->finalizeNoteEventASync(ctx.recordOnEvent);
    }

    if (ctx.isStepRecording || ctx.isRecMainWithoutPlaying)
    {
        auto newDuration = ctx.metronomeOnlyTickPosition - ctx.recordOnEvent->getTick();
        ctx.recordOnEvent->setTick(ctx.sequencerTickPosition);

        if (ctx.isStepRecording && ctx.isDurationOfRecordedNotesTcValue)
        {
            newDuration = static_cast<int>(ctx.noteValueLengthInTicks * (ctx.tcValuePercentage * 0.01));
            if (newDuration < 1) newDuration = 1;
        }

        const bool durationHasBeenAdjusted =
            ctx.activeTrack->finalizeNoteEventSynced(ctx.recordOnEvent, newDuration);

        if ((durationHasBeenAdjusted && ctx.isRecMainWithoutPlaying)
            || (ctx.isStepRecording && ctx.isAutoStepIncrementEnabled))
        {
            if (!ctx.isAnyProgramPadRegisteredAsPressed())
            {
                int nextPos = ctx.sequencerTickPosition + ctx.noteValueLengthInTicks;
                auto bar = ctx.currentBarIndex + 1;
                nextPos = ctx.activeTrack->timingCorrectTick(
                    0, bar, nextPos, ctx.noteValueLengthInTicks, ctx.swing);
                auto lastTick = ctx.sequencerGetActiveSequenceLastTick();

                if (nextPos != 0 && nextPos < lastTick)
                    ctx.sequencerMoveToQuarterNotePosition(sequencer::Sequencer::ticksToQuarterNotes(nextPos));
                else
                    ctx.sequencerMoveToQuarterNotePosition(sequencer::Sequencer::ticksToQuarterNotes(lastTick));
            }
        }
    }

    if (!ctx.isAnyProgramPadRegisteredAsPressed())
    {
        ctx.sequencerStopMetronomeTrack();
    }
}
