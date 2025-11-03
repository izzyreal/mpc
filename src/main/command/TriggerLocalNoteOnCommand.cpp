#include "TriggerLocalNoteOnCommand.hpp"

#include "audiomidi/EventHandler.hpp"
#include "command/context/TriggerLocalNoteOnContext.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/Sequencer.hpp"
#include "engine/PreviewSoundPlayer.hpp"
#include "Util.hpp"
#include "eventregistry/EventRegistry.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::command;
using namespace mpc::command::context;
using namespace mpc::sampler;

TriggerLocalNoteOnCommand::TriggerLocalNoteOnCommand(
    std::shared_ptr<TriggerLocalNoteOnContext> ctx)
    : ctx(ctx)
{
}

void TriggerLocalNoteOnCommand::execute()
{
    if (ctx->isSoundScreen)
    {
        ctx->previewSoundPlayer->mpcNoteOn(ctx->sampler->getSoundIndex(), 127,
                                           0);
        return;
    }

    const auto velo = ctx->isFullLevelEnabled ? 127 : ctx->velocity;

    const auto noteOnEvent =
        std::make_shared<sequencer::NoteOnEventPlayOnly>(ctx->note, velo);

    ctx->eventRegistry->registerProgramPadPress(
        ctx->source, ctx->screenComponent,
        ctx->sequencer->getBus<sequencer::Bus>(ctx->trackBus), ctx->program,
        ctx->programPadIndex, noteOnEvent->getVelocity(), ctx->track,
        std::nullopt);

    auto registryNoteOn = ctx->eventRegistry->registerNoteOn(
        ctx->source, ctx->screenComponent,
        ctx->sequencer->getBus<sequencer::Bus>(ctx->trackBus), ctx->note,
        noteOnEvent->getVelocity(), ctx->track, std::nullopt, ctx->program);

    if (ctx->isSequencerScreen && ctx->isNoteRepeatLockedOrPressed &&
        ctx->sequencer->isPlaying())
    {
        return;
    }

    if (ctx->isRecordingOrOverdubbing && ctx->isErasePressed)
    {
        return;
    }

    const bool is16LevelsEnabled = ctx->isSixteenLevelsEnabled;

    const auto assign16LevelsScreen = ctx->assign16LevelsScreen;

    Util::SixteenLevelsContext sixteenLevelsContext{
        is16LevelsEnabled,
        assign16LevelsScreen->getType(),
        assign16LevelsScreen->getOriginalKeyPad(),
        assign16LevelsScreen->getNote(),
        assign16LevelsScreen->getParameter(),
        ctx->programPadIndex % 16};

    Util::set16LevelsValues(sixteenLevelsContext, noteOnEvent);

    const bool isSliderNote =
        ctx->program && ctx->program->getSlider()->getNote() == ctx->note;
    auto programSlider = ctx->program->getSlider();

    Util::SliderNoteVariationContext sliderNoteVariationContext{
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
        noteOnEvent->setVariationType(type);
        noteOnEvent->setVariationValue(value);
    }

    if (ctx->isSamplerScreen)
    {
        ctx->eventHandler->handleUnfinalizedNoteOn(noteOnEvent, nullptr,
                                                   std::nullopt, std::nullopt,
                                                   ctx->drumScreenSelectedDrum);
    }
    else
    {
        const auto drumIndexToUse = ctx->trackBus > 0
                                        ? std::optional<int>(ctx->trackBus - 1)
                                        : std::nullopt;

        ctx->eventHandler->handleUnfinalizedNoteOn(
            noteOnEvent, ctx->track, ctx->track->getDeviceIndex(),
            ctx->track->getVelocityRatio(), drumIndexToUse);
    }

    std::shared_ptr<sequencer::NoteOnEvent> recordNoteOnEvent;

    if (ctx->sequencer->isRecordingOrOverdubbing())
    {
        recordNoteOnEvent = ctx->track->recordNoteEventASync(ctx->note, velo);
    }
    else if (ctx->isStepRecording &&
             (ctx->track->getBus() == 0 || sequencer::isDrumNote(ctx->note)))
    {
        recordNoteOnEvent = ctx->track->recordNoteEventSynced(
            ctx->sequencer->getTickPosition(), ctx->note, velo);
        ctx->sequencer->playMetronomeTrack();
        recordNoteOnEvent->setTick(
            ctx->frameSequencer->getMetronomeOnlyTickPosition());
    }
    else if (ctx->isRecMainWithoutPlaying)
    {
        recordNoteOnEvent = ctx->track->recordNoteEventSynced(
            ctx->sequencer->getTickPosition(), ctx->note, velo);
        ctx->sequencer->playMetronomeTrack();
        recordNoteOnEvent->setTick(
            ctx->frameSequencer->getMetronomeOnlyTickPosition());

        const auto timingCorrectScreen = ctx->timingCorrectScreen;
        const int stepLength = timingCorrectScreen->getNoteValueLengthInTicks();

        if (stepLength != 1)
        {
            const int bar = ctx->sequencer->getCurrentBarIndex() + 1;
            const auto correctedTick = ctx->track->timingCorrectTick(
                0, bar, ctx->sequencer->getTickPosition(), stepLength,
                timingCorrectScreen->getSwing());

            if (ctx->sequencer->getTickPosition() != correctedTick)
            {
                ctx->sequencer->move(
                    sequencer::Sequencer::ticksToQuarterNotes(correctedTick));
            }
        }
    }

    if (recordNoteOnEvent)
    {
        if (is16LevelsEnabled)
        {
            Util::set16LevelsValues(sixteenLevelsContext, recordNoteOnEvent);
        }

        if (ctx->program && isSliderNote)
        {
            auto [type, value] = Util::getSliderNoteVariationTypeAndValue(
                sliderNoteVariationContext);
            recordNoteOnEvent->setVariationType(type);
            recordNoteOnEvent->setVariationValue(value);
        }
    }

    if (recordNoteOnEvent)
    {
        registryNoteOn->recordNoteEvent = recordNoteOnEvent;
    }
}
