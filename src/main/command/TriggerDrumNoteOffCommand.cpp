#include "TriggerDrumNoteOffCommand.hpp"

#include "audiomidi/EventHandler.hpp"
#include "command/context/TriggerDrumNoteOffContext.hpp"
#include "eventregistry/EventRegistry.hpp"
#include "eventregistry/EventTypes.hpp"
#include "sequencer/NoteEvent.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::command;
using namespace mpc::command::context;

TriggerDrumNoteOffCommand::TriggerDrumNoteOffCommand(
    std::shared_ptr<TriggerDrumNoteOffContext> ctx)
    : ctx(ctx)
{
}

void TriggerDrumNoteOffCommand::execute()
{
    ctx->finishBasicVoiceIfSoundIsLooping();

    if (ctx->currentScreenIsSoundScreen)
    {
        return;
    }

    if (!ctx->noteOffEvent)
    {
        return;
    }

    if (ctx->currentScreenIsSamplerScreen)
    {
        ctx->eventHandler->handleNoteOffFromUnfinalizedNoteOn(
            ctx->noteOffEvent, nullptr, std::nullopt, ctx->drumIndex);
    }
    else
    {
        ctx->eventHandler->handleNoteOffFromUnfinalizedNoteOn(
            ctx->noteOffEvent, ctx->track, ctx->track->getDeviceIndex(),
            ctx->drumIndex);
    }

    ctx->eventRegistry->registerNoteOff(ctx->source, ctx->drumBus,
                                        ctx->noteOffEvent->getNote(),
                                        ctx->track, std::nullopt);

    std::function<void(void*)> action = [](void*){};

    if (ctx->recordOnEvent &&
            !(ctx->sequencerIsRecordingOrOverdubbing && ctx->isErasePressed))
    {
        action = [ctx = ctx](void *userData){

            auto programPadEvent = (eventregistry::ProgramPadPressEvent*)userData;

            const auto snapshot = ctx->eventRegistry->getSnapshot();

            const bool noMoreProgramPadsArePressed =
                snapshot.getTotalPressedProgramPadCount() == 0;

            if (ctx->sequencerIsRecordingOrOverdubbing)
            {
                ctx->track->finalizeNoteEventASync(ctx->recordOnEvent);
            }

            if (ctx->isStepRecording || ctx->isRecMainWithoutPlaying)
            {
                auto newDuration =
                    ctx->metronomeOnlyTickPosition - ctx->recordOnEvent->getTick();
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
                    ctx->track->finalizeNoteEventSynced(ctx->recordOnEvent,
                                                        newDuration);

                if ((durationHasBeenAdjusted && ctx->isRecMainWithoutPlaying) ||
                    (ctx->isStepRecording && ctx->isAutoStepIncrementEnabled))
                {
                    if (noMoreProgramPadsArePressed)
                    {
                        int nextPos =
                            ctx->sequencerTickPosition + ctx->noteValueLengthInTicks;

                        auto bar = ctx->currentBarIndex + 1;

                        nextPos = ctx->track->timingCorrectTick(
                            0, bar, nextPos, ctx->noteValueLengthInTicks, ctx->swing);

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
                                sequencer::Sequencer::ticksToQuarterNotes(lastTick));
                        }
                    }
                }
            }

            if (noMoreProgramPadsArePressed)
            {
                ctx->sequencerStopMetronomeTrack();
            }
        };
    }

    ctx->eventRegistry->registerProgramPadRelease(
        ctx->source, ctx->drumBus, ctx->program, ctx->programPadIndex,
        ctx->track, std::nullopt, action);
}

