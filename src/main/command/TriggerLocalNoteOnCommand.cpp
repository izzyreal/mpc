#include "TriggerLocalNoteOnCommand.hpp"
#include "sequencer/Transport.hpp"

#include "command/context/TriggerLocalNoteOnContext.hpp"

#include "audiomidi/EventHandler.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sampler/Program.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "Util.hpp"
#include "performance/PerformanceManager.hpp"
#include "sequencer/Track.hpp"
#include <memory>
#include <utility>

using namespace mpc::command;
using namespace mpc::command::context;
using namespace mpc::sampler;

TriggerLocalNoteOnCommand::TriggerLocalNoteOnCommand(
    std::shared_ptr<TriggerLocalNoteOnContext> ctx)
    : ctx(std::move(ctx))
{
}

void TriggerLocalNoteOnCommand::execute()
{
    const auto transport = ctx->sequencer.lock()->getTransport();

    if (ctx->isSequencerScreen && ctx->isNoteRepeatLockedOrPressed &&
        transport->isPlaying())
    {
        return;
    }

    if (ctx->isRecordingOrOverdubbing && ctx->isErasePressed)
    {
        return;
    }

    auto apply16LevelsAndSliderNoteVariation =
        [&](sequencer::EventData &noteEventToApplyTo)
    {
        if (ctx->program && ctx->programPadIndex)
        {
            const bool is16LevelsEnabled = ctx->isSixteenLevelsEnabled;

            const auto assign16LevelsScreen = ctx->assign16LevelsScreen;

            const Util::SixteenLevelsContext sixteenLevelsContext{
                is16LevelsEnabled,
                assign16LevelsScreen->getType(),
                assign16LevelsScreen->getOriginalKeyPad(),
                assign16LevelsScreen->getNote(),
                assign16LevelsScreen->getParameter(),
                *ctx->programPadIndex % 16};

            Util::set16LevelsValues(sixteenLevelsContext, noteEventToApplyTo);
        }

        if (ctx->program)
        {
            const bool isSliderNote =
                ctx->program &&
                ctx->program->getSlider()->getNote() == ctx->note;

            const auto programSlider = ctx->program->getSlider();

            const Util::SliderNoteVariationContext sliderNoteVariationContext{
                ctx->hardwareSliderValue,
                programSlider->getNote(),
                programSlider->getParameter(),
                programSlider->getTuneLowRange(),
                programSlider->getTuneHighRange(),
                programSlider->getDecayLowRange(),
                programSlider->getDecayHighRange(),
                programSlider->getAttackLowRange(),
                programSlider->getAttackHighRange(),
                programSlider->getFilterLowRange(),
                programSlider->getFilterHighRange()};

            if (ctx->program && isSliderNote)
            {
                auto [type, value] = Util::getSliderNoteVariationTypeAndValue(
                    sliderNoteVariationContext);
                noteEventToApplyTo.noteVariationType = type;
                noteEventToApplyTo.noteVariationValue =
                    NoteVariationValue(value);
            }
        }
    };

    const auto velo = ctx->isFullLevelEnabled ? MaxVelocity : ctx->velocity;

    sequencer::EventData noteOnEvent;
    noteOnEvent.type = sequencer::EventType::NoteOn;
    noteOnEvent.noteNumber = ctx->note;
    noteOnEvent.velocity = velo;

    apply16LevelsAndSliderNoteVariation(noteOnEvent);

    if (ctx->isSamplerScreen)
    {
        ctx->eventHandler->handleUnfinalizedNoteOn(noteOnEvent, nullptr,
                                                   std::nullopt, std::nullopt,
                                                   ctx->drumScreenSelectedDrum);
    }
    else
    {
        std::optional<sequencer::BusType> drumBusType = std::nullopt;

        if (const auto drumBus =
                std::dynamic_pointer_cast<sequencer::DrumBus>(ctx->bus);
            drumBus)
        {
            drumBusType =
                sequencer::drumBusIndexToDrumBusType(drumBus->getIndex());
        }

        ctx->eventHandler->handleUnfinalizedNoteOn(
            noteOnEvent, ctx->track, ctx->track->getDeviceIndex(),
            ctx->track->getVelocityRatio(), drumBusType);
    }

    sequencer::EventData* recordNoteOnEvent = nullptr;

    if (transport->isRecordingOrOverdubbing())
    {
        recordNoteOnEvent = ctx->track->recordNoteEventLive(ctx->note, velo);
    }
    else if (ctx->isStepRecording &&
             (sequencer::isMidiBusType(ctx->track->getBusType()) ||
              isDrumNote(ctx->note)))
    {
        transport->playMetronomeOnly();

        recordNoteOnEvent = ctx->track->recordNoteEventNonLive(
            ctx->positionTicks, ctx->note,
            velo, ctx->metronomeOnlyPositionTicks);
    }
    else if (ctx->isRecMainWithoutPlaying)
    {
        transport->playMetronomeOnly();

        recordNoteOnEvent = ctx->track->recordNoteEventNonLive(
            ctx->positionTicks, ctx->note,
            velo, ctx->metronomeOnlyPositionTicks);

        const auto timingCorrectScreen = ctx->timingCorrectScreen;

        if (const int stepLength =
                timingCorrectScreen->getNoteValueLengthInTicks();
            stepLength != 1)
        {
            const int bar =
                transport->getCurrentBarIndex() + 1;
            const auto correctedTick = ctx->track->timingCorrectTick(
                0, bar,
                ctx->positionTicks,
                stepLength, timingCorrectScreen->getSwing());

            if (ctx->positionTicks !=
                correctedTick)
            {
                transport->setPosition(
                    sequencer::Sequencer::ticksToQuarterNotes(correctedTick));
            }
        }
    }

    if (recordNoteOnEvent)
    {
        // TODO These changes still need to be committed. We should probably
        // construct the whole Event before enqueueing it.
        apply16LevelsAndSliderNoteVariation(*recordNoteOnEvent);

        ctx->registryNoteOnEvent.recordNoteEvent =
            recordNoteOnEvent;
    }
}
