#include "NoteRepeatProcessor.hpp"

#include "sequencer/Transport.hpp"
#include "eventregistry/EventRegistry.hpp"
#include "hardware/Component.hpp"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"
#include "engine/SequencerPlaybackEngine.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/NoteEvent.hpp"
#include "engine/DrumNoteEventHandler.hpp"
#include "engine/DrumNoteEventContextBuilder.hpp"
#include "sequencer/SeqUtil.hpp"
#include "Util.hpp"

using namespace mpc;
using namespace mpc::engine;
using namespace mpc::sequencer;
using namespace mpc::sampler;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::eventregistry;
using namespace mpc::engine::audio::mixer;

NoteRepeatProcessor::NoteRepeatProcessor(
    std::shared_ptr<Sequencer> s, std::shared_ptr<Sampler> sa,
    std::shared_ptr<AudioMixer> m, std::shared_ptr<Assign16LevelsScreen> a,
    std::shared_ptr<MixerSetupScreen> ms, std::shared_ptr<EventRegistry> e,
    std::shared_ptr<hardware::Slider> h, std::vector<std::shared_ptr<Voice>> *v,
    std::vector<MixerInterconnection *> &mi,
    const std::function<bool()> &isFullLevelEnabled,
    const std::function<bool()> &isSixteenLevelsEnabled)
    : sequencer(std::move(s)), sampler(std::move(sa)), mixer(std::move(m)),
      assign16LevelsScreen(std::move(a)), mixerSetupScreen(std::move(ms)),
      eventRegistry(std::move(e)), hardwareSlider(std::move(h)), voices(v),
      mixerConnections(mi), isFullLevelEnabled(isFullLevelEnabled),
      isSixteenLevelsEnabled(isSixteenLevelsEnabled)
{
}

void NoteRepeatProcessor::process(
    const SequencerPlaybackEngine *sequencerPlaybackEngine,
    unsigned int tickPosition, int durationTicks,
    const unsigned short eventFrameOffset, const double tempo,
    const float sampleRate) const
{
    auto track = sequencer->getActiveTrack();
    auto drumBus = sequencer->getBus<DrumBus>(track->getBusType());

    std::shared_ptr<Program> program;

    if (drumBus)
    {
        program = sampler->getProgram(drumBus->getProgram());
    }

    auto note = assign16LevelsScreen->getNote();

    const auto snapshot = eventRegistry->getSnapshot();

    static const std::vector sourcesToExclude{Source::NoteRepeat,
                                              Source::Sequence};

    for (int padIndex = 0; padIndex < Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX;
         ++padIndex)
    {
        if (!snapshot.getMostRecentProgramPadPress(padIndex, sourcesToExclude))
        {
            continue;
        }

        if (!isSixteenLevelsEnabled() && program)
        {
            note = program->getNoteFromPad(padIndex);
        }

        auto noteEvent = std::make_shared<sequencer::NoteOnEvent>(note);
        noteEvent->setTick(static_cast<int>(tickPosition));
        const bool isSliderNote =
            program && program->getSlider()->getNote() == note;

        if (program && isSliderNote)
        {
            const auto programSlider = program->getSlider();

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
            physicalPadPressInfo.bank ==
            controller::programPadIndexToBank(padIndex);

        if (isSixteenLevelsEnabled() && isPhysicallyPressed)
        {
            Util::SixteenLevelsContext sixteenLevelsContext{
                isSixteenLevelsEnabled(),
                assign16LevelsScreen->getType(),
                assign16LevelsScreen->getOriginalKeyPad(),
                assign16LevelsScreen->getNote(),
                assign16LevelsScreen->getParameter(),
                padIndex % 16};

            noteEvent->setVelocity(127);
            Util::set16LevelsValues(sixteenLevelsContext, noteEvent);
            note = noteEvent->getNote();
        }
        else
        {
            const int velocityToUseIfNotFullLevel =
                snapshot.getPressedProgramPadAfterTouchOrVelocity(padIndex);

            noteEvent->setVelocity(
                isFullLevelEnabled() ? 127 : velocityToUseIfNotFullLevel);
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
            const auto voiceOverlap =
                sound && sound->isLoopEnabled()
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

        if (sequencer->getTransport()->isRecordingOrOverdubbing())
        {
            noteEvent->setVelocity(velocityBeforeTrackVelocityRatioApplied);
            track->insertEventWhileRetainingSort(noteEvent);
        }

        sequencerPlaybackEngine->enqueueEventAfterNFrames(
            [voices = voices, track, note, tickPosition, drumBus]
            {
                if (drumBus)
                {
                    const auto ctx =
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
