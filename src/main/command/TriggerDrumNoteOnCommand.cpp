#include "TriggerDrumNoteOnCommand.hpp"

#include "audiomidi/EventHandler.hpp"
#include "command/context/TriggerDrumNoteOnContext.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/Sequencer.hpp"
#include "engine/PreviewSoundPlayer.hpp"
#include "sampler/Program.hpp"
#include "sampler/Pad.hpp"
#include "Util.hpp"

using namespace mpc::command;
using namespace mpc::command::context;
using namespace mpc::sampler;

TriggerDrumNoteOnCommand::TriggerDrumNoteOnCommand(
    TriggerDrumNoteOnContext &ctx)
    : ctx(ctx)
{
}

void TriggerDrumNoteOnCommand::execute()
{
    if (ctx.isSoundScreen)
    {
        ctx.basicPlayer.mpcNoteOn(ctx.soundIndex, 127, 0);
        return;
    }

    if (ctx.isSequencerScreen && ctx.isNoteRepeatLockedOrPressed &&
        ctx.sequencer->isPlaying() && ctx.isPhysicallyPressed)
    {
        return;
    }

    if (ctx.isRecordingOrOverdubbing && ctx.isErasePressed)
    {
        return;
    }

    generateNoteOn(ctx);
}

void TriggerDrumNoteOnCommand::generateNoteOn(
    const TriggerDrumNoteOnContext &ctx)
{
    const bool is16LevelsEnabled = ctx.isSixteenLevelsEnabled;

    const auto velo = ctx.isFullLevelEnabled ? 127 : ctx.velocity;

    const auto playOnEvent =
        std::make_shared<sequencer::NoteOnEventPlayOnly>(ctx.note, velo);

    const auto assign16LevelsScreen = ctx.assign16LevelsScreen;

    Util::SixteenLevelsContext sixteenLevelsContext{
        is16LevelsEnabled,
        assign16LevelsScreen->getType(),
        assign16LevelsScreen->getOriginalKeyPad(),
        assign16LevelsScreen->getNote(),
        assign16LevelsScreen->getParameter(),
        ctx.programPadIndex % 16};

    Util::set16LevelsValues(sixteenLevelsContext, playOnEvent);

    const bool isSliderNote =
        ctx.program && ctx.program->getSlider()->getNote() == ctx.note;
    auto programSlider = ctx.program->getSlider();

    Util::SliderNoteVariationContext sliderNoteVariationContext{
        ctx.hardwareSliderValue,
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

    if (ctx.program && isSliderNote)
    {
        Util::setSliderNoteVariationParameters(sliderNoteVariationContext,
                                               playOnEvent);
    }

    ctx.sequencer->getNoteEventStore().storePlayNoteEvent(ctx.programPadIndex,
                                                          playOnEvent);

    if (ctx.isSamplerScreen)
    {
        ctx.eventHandler->handleUnfinalizedNoteOn(playOnEvent, std::nullopt,
                                                  std::nullopt, std::nullopt,
                                                  ctx.drumScreenSelectedDrum);
    }
    else
    {
        const auto drumIndexToUse = ctx.trackBus > 0
                                        ? std::optional<int>(ctx.trackBus - 1)
                                        : std::nullopt;

        ctx.eventHandler->handleUnfinalizedNoteOn(
            playOnEvent, ctx.track->getIndex(), ctx.track->getDeviceIndex(),
            ctx.track->getVelocityRatio(), drumIndexToUse);
    }

    std::shared_ptr<sequencer::NoteOnEvent> recordNoteOnEvent;

    if (ctx.sequencer->isRecordingOrOverdubbing())
    {
        recordNoteOnEvent = ctx.track->recordNoteEventASync(ctx.note, velo);
    }
    else if (ctx.isStepRecording &&
             (ctx.track->getBus() == 0 || sequencer::isDrumNote(ctx.note)))
    {
        recordNoteOnEvent = ctx.track->recordNoteEventSynced(
            ctx.sequencer->getTickPosition(), ctx.note, velo);
        ctx.sequencer->playMetronomeTrack();
        recordNoteOnEvent->setTick(
            ctx.frameSequencer->getMetronomeOnlyTickPosition());
    }
    else if (ctx.isRecMainWithoutPlaying)
    {
        recordNoteOnEvent = ctx.track->recordNoteEventSynced(
            ctx.sequencer->getTickPosition(), ctx.note, velo);
        ctx.sequencer->playMetronomeTrack();
        recordNoteOnEvent->setTick(
            ctx.frameSequencer->getMetronomeOnlyTickPosition());

        const auto timingCorrectScreen = ctx.timingCorrectScreen;
        const int stepLength = timingCorrectScreen->getNoteValueLengthInTicks();

        if (stepLength != 1)
        {
            const int bar = ctx.sequencer->getCurrentBarIndex() + 1;
            const auto correctedTick = ctx.track->timingCorrectTick(
                0, bar, ctx.sequencer->getTickPosition(), stepLength,
                timingCorrectScreen->getSwing());

            if (ctx.sequencer->getTickPosition() != correctedTick)
            {
                ctx.sequencer->move(
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

        if (ctx.program && isSliderNote)
        {
            Util::setSliderNoteVariationParameters(sliderNoteVariationContext,
                                                   recordNoteOnEvent);
        }

        ctx.sequencer->getNoteEventStore().storeRecordNoteEvent(
            ctx.programPadIndex, recordNoteOnEvent);
    }
}
