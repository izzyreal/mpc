#include "TriggerDrumNoteOffCommand.hpp"

#include "audiomidi/EventHandler.hpp"
#include "command/context/TriggerDrumNoteOffContext.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::command;
using namespace mpc::command::context;

TriggerDrumNoteOffCommand::TriggerDrumNoteOffCommand(TriggerDrumNoteOffContext& ctx)
    : ctx(ctx) {}

void TriggerDrumNoteOffCommand::execute()
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

    if (ctx.currentScreenIsSamplerScreen)
    {
        ctx.eventHandler->handleNoteOffFromUnfinalizedNoteOn(ctx.playNoteEvent->getNoteOff(), std::nullopt, std::nullopt, ctx.drumScreenSelectedDrum);
    }
    else
    {
        const auto drumIndexToUse = ctx.activeTrack->getBus() > 0 ? std::optional<int>(ctx.activeTrack->getBus() - 1) : std::nullopt;

        ctx.eventHandler->handleNoteOffFromUnfinalizedNoteOn(ctx.playNoteEvent->getNoteOff(),
                                                             ctx.activeTrack->getIndex(),
                                                             ctx.activeTrack->getDeviceIndex(),
                                                             drumIndexToUse);
    }

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

                nextPos = ctx.activeTrack->timingCorrectTick(0, bar, nextPos, ctx.noteValueLengthInTicks, ctx.swing);

                auto lastTick = ctx.sequencerGetActiveSequenceLastTick();

                if (nextPos != 0 && nextPos < lastTick)
                {
                    const double nextPosQuarterNotes = sequencer::Sequencer::ticksToQuarterNotes(nextPos);
                    ctx.sequencerMoveToQuarterNotePosition(nextPosQuarterNotes);
                }
                else
                {
                    ctx.sequencerMoveToQuarterNotePosition(sequencer::Sequencer::ticksToQuarterNotes(lastTick));
                }
            }
        }
    }

    if (!ctx.isAnyProgramPadRegisteredAsPressed())
    {
        ctx.sequencerStopMetronomeTrack();
    }
}

