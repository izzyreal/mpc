#include "TriggerLocalNoteOffCommand.hpp"

#include "audiomidi/EventHandler.hpp"
#include "command/context/TriggerLocalNoteOffContext.hpp"
#include "performance/PerformanceManager.hpp"
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
    std::optional<DrumBusIndex> drumIndex = std::nullopt;

    if (const auto drumBus =
            std::dynamic_pointer_cast<sequencer::DrumBus>(ctx->bus);
        drumBus)
    {
        drumIndex = drumBus->getIndex();
    }

    ctx->eventHandler->handleNoteOffFromUnfinalizedNoteOn(
        ctx->noteNumber, ctx->track->getDeviceIndex(), drumIndex);

    if (ctx->recordOnEvent &&
        !(ctx->sequencerIsRecordingOrOverdubbing && ctx->isErasePressed))
    {
        const auto snapshot = ctx->performanceManager.lock()->getSnapshot();

        const bool thisIsTheLastActiveNoteOn =
            snapshot.getTotalNoteOnCount() == 0;

        if (ctx->sequencerIsRecordingOrOverdubbing)
        {
            ctx->track->finalizeNoteEventLive(ctx->recordOnEvent,
                                              ctx->positionTicks);
        }

        if (ctx->isStepRecording || ctx->isRecMainWithoutPlaying)
        {
            auto newDuration = ctx->metronomeOnlyPositionTicks -
                               ctx->recordOnEvent->metronomeOnlyTickPosition;
            ctx->recordOnEvent->tick = ctx->sequencerPositionTicks;

            if (ctx->isStepRecording && ctx->isDurationOfRecordedNotesTcValue)
            {
                newDuration = static_cast<int>(ctx->noteValueLengthInTicks *
                                               (ctx->tcValuePercentage * 0.01));
                if (newDuration < 1)
                {
                    newDuration = 1;
                }
            }

            ctx->track->finalizeNoteEventNonLive(ctx->recordOnEvent,
                                                 Duration(newDuration));

            if (((ctx->isStepRecording && ctx->isAutoStepIncrementEnabled) ||
                 ctx->isRecMainWithoutPlaying) &&
                thisIsTheLastActiveNoteOn)
            {
                int nextPos =
                    ctx->sequencerPositionTicks + ctx->noteValueLengthInTicks;

                const auto bar = ctx->currentBarIndex + 1;

                nextPos = ctx->track->timingCorrectTick(
                    0, bar, nextPos, ctx->noteValueLengthInTicks, ctx->swing);

                if (const auto lastTick =
                        ctx->sequencerGetActiveSequenceLastTick();
                    nextPos != 0 && nextPos < lastTick)
                {
                    const double nextPosQuarterNotes =
                        sequencer::Sequencer::ticksToQuarterNotes(nextPos);
                    ctx->sequencerMoveToQuarterNotePosition(
                        nextPosQuarterNotes);
                }
                else
                {
                    ctx->sequencerMoveToQuarterNotePosition(
                        sequencer::Sequencer::ticksToQuarterNotes(lastTick));
                }
            }
        }

        if (thisIsTheLastActiveNoteOn)
        {
            ctx->sequencerStopMetronomeTrack();
        }
    }
}
