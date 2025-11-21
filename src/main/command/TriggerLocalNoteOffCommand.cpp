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

    if (ctx->currentScreenIsSamplerScreen)
    {
        ctx->eventHandler->handleNoteOffFromUnfinalizedNoteOn(
            ctx->noteNumber, nullptr, std::nullopt, drumIndex);
    }
    else
    {
        ctx->eventHandler->handleNoteOffFromUnfinalizedNoteOn(
            ctx->noteNumber, ctx->track, ctx->track->getDeviceIndex(),
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
            ctx->track->finalizeNoteEventLive(*ctx->recordOnEvent);
        }

        if (ctx->isStepRecording || ctx->isRecMainWithoutPlaying)
        {
            auto newDuration = ctx->metronomeOnlyTickPosition -
                               ctx->recordOnEvent->metronomeOnlyTickPosition;
            ctx->recordOnEvent->tick = ctx->sequencerTickPosition;

            if (ctx->isStepRecording && ctx->isDurationOfRecordedNotesTcValue)
            {
                newDuration = static_cast<int>(ctx->noteValueLengthInTicks *
                                               (ctx->tcValuePercentage * 0.01));
                if (newDuration < 1)
                {
                    newDuration = 1;
                }
            }

            const Duration oldDuration = ctx->recordOnEvent->duration;

            ctx->track->finalizeNoteEventNonLive(*ctx->recordOnEvent, Duration(newDuration));

            if ((oldDuration != ctx->recordOnEvent->duration &&
                 ctx->isRecMainWithoutPlaying) ||
                (ctx->isStepRecording && ctx->isAutoStepIncrementEnabled))
            {
                if (thisIsTheLastActiveNoteOn)
                {
                    int nextPos = ctx->sequencerTickPosition +
                                  ctx->noteValueLengthInTicks;

                    const auto bar = ctx->currentBarIndex + 1;

                    nextPos = ctx->track->timingCorrectTick(
                        0, bar, nextPos, ctx->noteValueLengthInTicks,
                        ctx->swing);

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
