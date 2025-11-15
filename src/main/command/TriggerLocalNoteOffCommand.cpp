#include "TriggerLocalNoteOffCommand.hpp"

#include "audiomidi/EventHandler.hpp"
#include "command/context/TriggerLocalNoteOffContext.hpp"
#include "performance/PerformanceManager.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Bus.hpp"
#include <memory>

using namespace mpc::command;
using namespace mpc::command::context;

TriggerLocalNoteOffCommand::TriggerLocalNoteOffCommand(
    const std::shared_ptr<TriggerLocalNoteOffContext> &ctx)
    : ctx(ctx)
{
}

void TriggerLocalNoteOffCommand::execute()
{
    if (!ctx->noteOffEvent)
    {
        return;
    }

    std::optional<DrumBusIndex> drumIndex = std::nullopt;

    if (const auto drumBus =
            std::dynamic_pointer_cast<sequencer::DrumBus>(ctx->bus);
        drumBus)
    {
        drumIndex = drumBus->getIndex();
    }

    if (ctx->currentScreenIsSamplerScreen)
    {
        ctx->eventHandler->handleNoteOffFromUnfinalizedNoteOn(
            ctx->noteOffEvent, nullptr, std::nullopt, drumIndex);
    }
    else
    {
        ctx->eventHandler->handleNoteOffFromUnfinalizedNoteOn(
            ctx->noteOffEvent, ctx->track, ctx->track->getDeviceIndex(),
            drumIndex);
    }

    if (ctx->recordOnEvent &&
        !(ctx->sequencerIsRecordingOrOverdubbing && ctx->isErasePressed))
    {
        const auto snapshot = ctx->performanceManager->getSnapshot();

        const bool thisIsTheLastActiveNoteOn =
            snapshot.getTotalNoteOnCount() == 1;

        if (ctx->sequencerIsRecordingOrOverdubbing)
        {
            ctx->track->finalizeNoteEventLive(ctx->recordOnEvent);
        }

        if (ctx->isStepRecording || ctx->isRecMainWithoutPlaying)
        {
            auto newDuration =
                ctx->metronomeOnlyTickPosition -
                ctx->recordOnEvent->getMetronomeOnlyTickPosition();
            ctx->recordOnEvent->setTick(ctx->sequencerTickPosition);

            if (ctx->isStepRecording && ctx->isDurationOfRecordedNotesTcValue)
            {
                newDuration = static_cast<int>(ctx->noteValueLengthInTicks *
                                               (ctx->tcValuePercentage * 0.01));
                if (newDuration < 1)
                {
                    newDuration = 1;
                }
            }

            const bool durationHasBeenAdjusted =
                ctx->recordOnEvent->finalizeNonLive(newDuration);

            if ((durationHasBeenAdjusted && ctx->isRecMainWithoutPlaying) ||
                (ctx->isStepRecording && ctx->isAutoStepIncrementEnabled))
            {
                if (thisIsTheLastActiveNoteOn)
                {
                    int nextPos = ctx->sequencerTickPosition +
                                  ctx->noteValueLengthInTicks;

                    auto bar = ctx->currentBarIndex + 1;

                    nextPos = ctx->track->timingCorrectTick(
                        0, bar, nextPos, ctx->noteValueLengthInTicks,
                        ctx->swing);

                    auto lastTick = ctx->sequencerGetActiveSequenceLastTick();

                    if (nextPos != 0 && nextPos < lastTick)
                    {
                        const double nextPosQuarterNotes =
                            sequencer::Sequencer::ticksToQuarterNotes(nextPos);
                        ctx->sequencerMoveToQuarterNotePosition(
                            nextPosQuarterNotes);
                    }
                    else
                    {
                        ctx->sequencerMoveToQuarterNotePosition(
                            sequencer::Sequencer::ticksToQuarterNotes(
                                lastTick));
                    }
                }
            }
        }

        if (thisIsTheLastActiveNoteOn)
        {
            ctx->sequencerStopMetronomeTrack();
        }
    }
}
