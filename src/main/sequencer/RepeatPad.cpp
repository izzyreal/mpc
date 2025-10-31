#include "RepeatPad.hpp"

#include "Mpc.hpp"
#include "Util.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/MidiOutput.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
#include "eventregistry/EventRegistry.hpp"
#include "hardware/Hardware.hpp"
#include "engine/DrumNoteEventHandler.hpp"
#include "engine/DrumNoteEventContextBuilder.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;
using namespace mpc::engine;

void RepeatPad::process(mpc::Mpc &mpc, unsigned int tickPosition,
                        int durationTicks, unsigned short eventFrameOffset,
                        double tempo, float sampleRate)
{
    if (mpc.getLayeredScreen()->getCurrentScreenName() != "sequencer")
    {
        return;
    }

    auto sequencer = mpc.getSequencer();
    auto track = sequencer->getActiveTrack();
    auto drumBus = sequencer->getBus<DrumBus>(track->getBus());

    std::shared_ptr<mpc::sampler::Program> program;

    if (drumBus)
    {
        program = mpc.getSampler()->getProgram(drumBus->getProgram());
    }

    const auto fullLevel = mpc.clientEventController->isFullLevelEnabled();
    const auto sixteenLevels =
        mpc.clientEventController->isSixteenLevelsEnabled();

    auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>();
    auto note = assign16LevelsScreen->getNote();

    auto snapshot = mpc.eventRegistry->getSnapshot();

    static const std::vector<eventregistry::Source> sourcesToExclude { eventregistry::Source::NoteRepeat, eventregistry::Source::Sequence };

    for (int padIndex = 0; padIndex < Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX;
         ++padIndex)
    {
        if (!snapshot.isProgramPadPressed(padIndex, sourcesToExclude))
        {
            continue;
        }

        if (!sixteenLevels && program)
        {
            note = program->getNoteFromPad(padIndex);
        }

        auto noteEvent = std::make_shared<NoteOnEvent>(note);
        noteEvent->setTick(static_cast<int>(tickPosition));
        const bool isSliderNote =
            program && program->getSlider()->getNote() == note;

        if (program && isSliderNote)
        {
            auto hardwareSlider = mpc.getHardware()->getSlider();
            auto programSlider = program->getSlider();

            Util::SliderNoteVariationContext sliderNoteVariationContext{
                hardwareSlider->getValueAs<int>(),
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

            auto [type, value] = Util::getSliderNoteVariationTypeAndValue(
                sliderNoteVariationContext);
            noteEvent->setVariationType(type);
            noteEvent->setVariationValue(value);
        }

        const auto physicalPadPressInfo =
            snapshot.retrievePhysicalPadPressEvent(padIndex % 16);

        const bool isPhysicallyPressed =
            physicalPadPressInfo && physicalPadPressInfo->bank == padIndex / 16;

        if (sixteenLevels && isPhysicallyPressed)
        {
            Util::SixteenLevelsContext sixteenLevelsContext{
                sixteenLevels,
                assign16LevelsScreen->getType(),
                assign16LevelsScreen->getOriginalKeyPad(),
                assign16LevelsScreen->getNote(),
                assign16LevelsScreen->getParameter(),
                padIndex % 16};

            noteEvent->setVelocity(127);
            mpc::Util::set16LevelsValues(sixteenLevelsContext, noteEvent);
            note = noteEvent->getNote();
        }
        else
        {
            const int velocityToUseIfNotFullLevel =
                snapshot.getPressedProgramPadAfterTouchOrVelocity(padIndex);

            noteEvent->setVelocity(fullLevel ? 127
                                             : velocityToUseIfNotFullLevel);
        }

        noteEvent->setDuration(durationTicks);

        noteEvent->getNoteOff()->setTick(static_cast<int>(tickPosition) +
                                         durationTicks);

        const auto velocityBeforeTrackVelocityRatioApplied =
            noteEvent->getVelocity();

        noteEvent->setVelocity(noteEvent->getVelocity() *
                               (track->getVelocityRatio() * 0.01));

        const auto durationFrames = static_cast<int>(
            durationTicks == -1
                ? -1
                : SeqUtil::ticksToFrames(durationTicks, tempo, sampleRate));

        if (drumBus && note != 34)
        {
            const auto noteParameters = program->getNoteParameters(note);
            const auto sound =
                mpc.getSampler()->getSound(noteParameters->getSoundIndex());
            auto voiceOverlap = (sound && sound->isLoopEnabled())
                                    ? VoiceOverlapMode::NOTE_OFF
                                    : noteParameters->getVoiceOverlapMode();

            auto ctx = DrumNoteEventContextBuilder::buildNoteOn(
                0, drumBus, mpc.getSampler(),
                mpc.getAudioMidiServices()->getMixer(),
                mpc.screens->get<MixerSetupScreen>(),
                &mpc.getAudioMidiServices()->getVoices(),
                mpc.getAudioMidiServices()->getMixerConnections(), note,
                noteEvent->getVelocity(), noteEvent->getVariationType(),
                noteEvent->getVariationValue(), eventFrameOffset,
                /* firstGeneration */ true, // Always true for invokers that
                                            // are not
                                            // DrumNoteEventHandler::noteOn
                                            // itself
                /*tick*/ -1,
                voiceOverlap == VoiceOverlapMode::NOTE_OFF ? durationFrames
                                                           : -1);

            DrumNoteEventHandler::noteOn(ctx);
        }

        if (track->getDeviceIndex() > 0)
        {
            const auto noteOnMsg = noteEvent->createShortMessage(
                (track->getDeviceIndex() - 1) % 16);
            noteOnMsg->bufferPos = eventFrameOffset;
            mpc.getMidiOutput()->enqueueMessageOutputA(noteOnMsg);
        }

        if (sequencer->isRecordingOrOverdubbing())
        {
            noteEvent->setVelocity(velocityBeforeTrackVelocityRatioApplied);
            track->insertEventWhileRetainingSort(noteEvent);
        }

        mpc.getAudioMidiServices()
            ->getFrameSequencer()
            ->enqueueEventAfterNFrames(
                [&mpc, track, note, noteEvent, tickPosition, program, padIndex,
                 drumBus]
                {
                    if (drumBus)
                    {
                        auto ctx = DrumNoteEventContextBuilder::buildNoteOff(
                            0, drumBus,
                            &mpc.getAudioMidiServices()->getVoices(), note,
                            tickPosition);

                        DrumNoteEventHandler::noteOff(ctx);
                    }

                    if (track->getDeviceIndex() > 0)
                    {
                        const auto noteOffMsg =
                            noteEvent->getNoteOff()->createShortMessage(
                                (track->getDeviceIndex() - 1) % 16);
                        mpc.getMidiOutput()->enqueueMessageOutputA(noteOffMsg);
                    }
                },
                durationFrames - 1);
    }
}
