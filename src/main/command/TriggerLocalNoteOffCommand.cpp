#include "TriggerLocalNoteOffCommand.hpp"

#include "audiomidi/EventHandler.hpp"
#include "command/context/TriggerLocalNoteOffContext.hpp"
#include "performance/PerformanceManager.hpp"
#include "sequencer/SequenceStateView.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/Transport.hpp"

using namespace mpc::command;
using namespace mpc::command::context;

TriggerLocalNoteOffCommand::TriggerLocalNoteOffCommand(
    const TriggerLocalNoteOffContext &ctx)
    : ctx(ctx)
{
}

void TriggerLocalNoteOffCommand::execute()
{
    std::optional<DrumBusIndex> drumIndex = std::nullopt;

    if (const auto drumBus = dynamic_cast<sequencer::DrumBus *>(ctx.bus);
        drumBus)
    {
        drumIndex = drumBus->getIndex();
    }

    ctx.eventHandler->handleNoteOffFromUnfinalizedNoteOn(
        ctx.noteNumber, ctx.track->getDeviceIndex(), drumIndex);

    if (ctx.recordOnEvent &&
        !(ctx.sequencerIsRecordingOrOverdubbing && ctx.isErasePressed))
    {
        const auto snapshot = ctx.performanceManager->getSnapshot();

        const bool thisIsTheLastActiveNoteOn =
            snapshot.getTotalNoteOnCount() == 0;

        if (ctx.sequencerIsRecordingOrOverdubbing)
        {
            ctx.sequencerStateManager->finalizeNoteEventLive(
                ctx.recordOnEvent, ctx.positionTicks);
        }

        if (ctx.isStepRecording || ctx.isRecMainWithoutPlaying)
        {
            auto newDuration = ctx.metronomeOnlyPositionTicks -
                               ctx.recordOnEvent->metronomeOnlyTickPosition;
            ctx.recordOnEvent->tick = ctx.sequencerPositionTicks;

            if (ctx.isStepRecording && ctx.isDurationOfRecordedNotesTcValue)
            {
                newDuration = static_cast<int>(ctx.noteValueLengthInTicks *
                                               (ctx.tcValuePercentage * 0.01));
                if (newDuration < 1)
                {
                    newDuration = 1;
                }
            }

            ctx.track->finalizeNoteEventNonLive(ctx.recordOnEvent,
                                                 Duration(newDuration));

            if (((ctx.isStepRecording && ctx.isAutoStepIncrementEnabled) ||
                 ctx.isRecMainWithoutPlaying) &&
                thisIsTheLastActiveNoteOn)
            {
                int nextPos =
                    ctx.sequencerPositionTicks + ctx.noteValueLengthInTicks;

                const auto bar = ctx.currentBarIndex + 1;

                const auto sequenceStateView =
                    ctx.sequencerStateManager->getSnapshot().getSequenceState(
                        ctx.track->getSequenceIndex());

                nextPos = ctx.track->timingCorrectTick(
                    sequenceStateView, 0, bar, nextPos,
                    ctx.noteValueLengthInTicks, ctx.swing);

                if (const auto lastTick = sequenceStateView.getLastTick();
                    nextPos != 0 && nextPos < lastTick)
                {
                    const double nextPosQuarterNotes =
                        sequencer::Sequencer::ticksToQuarterNotes(nextPos);
                    ctx.sequencer->getTransport()->setPosition(
                        nextPosQuarterNotes);
                }
                else
                {
                    ctx.sequencer->getTransport()->setPosition(
                        sequencer::Sequencer::ticksToQuarterNotes(lastTick));
                }
            }
        }

        if (thisIsTheLastActiveNoteOn)
        {
            ctx.sequencer->getTransport()->stopMetronomeOnly();
        }
    }
}
