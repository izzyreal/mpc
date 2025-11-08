#include "NoteRepeatProcessor.hpp"

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
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::eventregistry;

void NoteRepeatProcessor::process(
    FrameSeq *frameSequencer, Sequencer *sequencer,
    std::shared_ptr<Sampler> sampler, std::shared_ptr<AudioMixer> mixer,
    bool isFullLevelEnabled, bool isSixteenLevelsEnabled,
    std::shared_ptr<Assign16LevelsScreen> assign16LevelsScreen,
    std::shared_ptr<MixerSetupScreen> mixerSetupScreen,
    std::shared_ptr<EventRegistry> eventRegistry,
    std::shared_ptr<hardware::Slider> hardwareSlider,
    std::vector<std::shared_ptr<Voice>> *voices,
    std::vector<MixerInterconnection *> &mixerConnections,
    unsigned int tickPosition, int durationTicks,
    unsigned short eventFrameOffset, double tempo, float sampleRate)
{
    auto track = sequencer->getActiveTrack();
    auto drumBus = sequencer->getBus<DrumBus>(track->getBus());

    std::shared_ptr<mpc::sampler::Program> program;

    if (drumBus)
    {
        program = sampler->getProgram(drumBus->getProgram());
    }

    auto note = assign16LevelsScreen->getNote();

    auto snapshot = eventRegistry->getSnapshot();

    static const std::vector<eventregistry::Source> sourcesToExclude{
        eventregistry::Source::NoteRepeat, eventregistry::Source::Sequence};

    for (int padIndex = 0; padIndex < Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX;
         ++padIndex)
    {
        if (!snapshot.getMostRecentProgramPadPress(padIndex, sourcesToExclude))
        {
            continue;
        }

        if (!isSixteenLevelsEnabled && program)
        {
            note = program->getNoteFromPad(padIndex);
        }

        auto noteEvent = std::make_shared<NoteOnEvent>(note);
        noteEvent->setTick(static_cast<int>(tickPosition));
        const bool isSliderNote =
            program && program->getSlider()->getNote() == note;

        if (program && isSliderNote)
        {
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

        if (isSixteenLevelsEnabled && isPhysicallyPressed)
        {
            Util::SixteenLevelsContext sixteenLevelsContext{
                isSixteenLevelsEnabled,
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

            noteEvent->setVelocity(
                isFullLevelEnabled ? 127 : velocityToUseIfNotFullLevel);
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
                sampler->getSound(noteParameters->getSoundIndex());
            auto voiceOverlap = (sound && sound->isLoopEnabled())
                                    ? VoiceOverlapMode::NOTE_OFF
                                    : noteParameters->getVoiceOverlapMode();

            auto ctx = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
                0, drumBus, sampler, mixer, mixerSetupScreen, voices,
                mixerConnections, note, noteEvent->getVelocity(),
                noteEvent->getVariationType(), noteEvent->getVariationValue(),
                eventFrameOffset,
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
            // const auto noteOnMsg = noteEvent->createShortMessage(
            //(track->getDeviceIndex() - 1) % 16);
            // noteOnMsg->bufferPos = eventFrameOffset;
            // mpc.getMidiOutput()->enqueueMessageOutputA(noteOnMsg);
        }

        if (sequencer->isRecordingOrOverdubbing())
        {
            noteEvent->setVelocity(velocityBeforeTrackVelocityRatioApplied);
            track->insertEventWhileRetainingSort(noteEvent);
        }

        frameSequencer->enqueueEventAfterNFrames(
            [voices, track, note, noteEvent, tickPosition, program, padIndex,
             drumBus]
            {
                if (drumBus)
                {
                    auto ctx =
                        DrumNoteEventContextBuilder::buildDrumNoteOffContext(
                            0, drumBus, voices, note, tickPosition);

                    DrumNoteEventHandler::noteOff(ctx);
                }

                if (track->getDeviceIndex() > 0)
                {
                    // const auto noteOffMsg =
                    // noteEvent->getNoteOff()->createShortMessage(
                    //(track->getDeviceIndex() - 1) % 16);
                    // mpc.getMidiOutput()->enqueueMessageOutputA(noteOffMsg);
                }
            },
            durationFrames - 1);
    }
}
