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
    std::function<bool()> &isFullLevelEnabled,
    std::function<bool()> &isSixteenLevelsEnabled)
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
    const auto track = sequencer->getActiveTrack();
    auto drumBus = sequencer->getBus<DrumBus>(track->getBus());

    std::shared_ptr<Program> program;
    if (drumBus)
    {
        program = sampler->getProgram(drumBus->getProgram());
    }

    auto note = assign16LevelsScreen->getNote();
    const auto snapshot = eventRegistry->getSnapshot();

    static const std::vector exclude{Source::NoteRepeat, Source::Sequence};

    for (int pad = 0; pad < Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX; ++pad)
    {
        if (!snapshot.getMostRecentProgramPadPress(pad, exclude))
        {
            continue;
        }

        if (!isSixteenLevelsEnabled && program)
        {
            note = program->getNoteFromPad(pad);
        }

        auto noteEvent = std::make_shared<sequencer::NoteOnEvent>(note);
        noteEvent->setTick(static_cast<int>(tickPosition));

        const bool isSliderNote =
            program && program->getSlider()->getNote() == note;

        if (program && isSliderNote)
        {
            Util::SliderNoteVariationContext ctx{
                hardwareSlider->getValueAs<int>(),
                program->getSlider()->getNote(),
                program->getSlider()->getParameter(),
                program->getSlider()->getTuneLowRange(),
                program->getSlider()->getTuneHighRange(),
                program->getSlider()->getDecayLowRange(),
                program->getSlider()->getDecayHighRange(),
                program->getSlider()->getAttackLowRange(),
                program->getSlider()->getAttackHighRange(),
                program->getSlider()->getFilterLowRange(),
                program->getSlider()->getFilterHighRange()};

            auto [type, value] = Util::getSliderNoteVariationTypeAndValue(ctx);
            noteEvent->setVariationType(type);
            noteEvent->setVariationValue(value);
        }

        const auto physicalPadPress =
            snapshot.retrievePhysicalPadPressEvent(pad % 16);
        const bool isPhysicallyPressed =
            physicalPadPress && physicalPadPress->bank == pad / 16;

        if (isSixteenLevelsEnabled() && isPhysicallyPressed)
        {
            Util::SixteenLevelsContext ctx{
                isSixteenLevelsEnabled(),
                assign16LevelsScreen->getType(),
                assign16LevelsScreen->getOriginalKeyPad(),
                assign16LevelsScreen->getNote(),
                assign16LevelsScreen->getParameter(),
                pad % 16};

            noteEvent->setVelocity(127);
            Util::set16LevelsValues(ctx, noteEvent);
            note = noteEvent->getNote();
        }
        else
        {
            const int velocity =
                snapshot.getPressedProgramPadAfterTouchOrVelocity(pad);
            noteEvent->setVelocity(isFullLevelEnabled() ? 127 : velocity);
        }

        noteEvent->setDuration(durationTicks);
        noteEvent->getNoteOff()->setTick(static_cast<int>(tickPosition) +
                                         durationTicks);

        const auto velBeforeRatio = noteEvent->getVelocity();
        noteEvent->setVelocity(noteEvent->getVelocity() *
                               (track->getVelocityRatio() * 0.01));

        const auto durationFrames = static_cast<int>(
            durationTicks == -1
                ? -1
                : SeqUtil::ticksToFrames(durationTicks, tempo, sampleRate));

        if (drumBus && note != 34)
        {
            const auto noteParams = program->getNoteParameters(note);
            const auto sound = sampler->getSound(noteParams->getSoundIndex());
            const auto overlap = sound && sound->isLoopEnabled()
                                     ? VoiceOverlapMode::NOTE_OFF
                                     : noteParams->getVoiceOverlapMode();

            auto ctx = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
                0, drumBus, sampler, mixer, mixerSetupScreen, voices,
                mixerConnections, note, noteEvent->getVelocity(),
                noteEvent->getVariationType(), noteEvent->getVariationValue(),
                eventFrameOffset, true, -1,
                overlap == VoiceOverlapMode::NOTE_OFF ? durationFrames : -1);

            DrumNoteEventHandler::noteOn(ctx);
        }

        if (sequencer->getTransport()->isRecordingOrOverdubbing())
        {
            noteEvent->setVelocity(velBeforeRatio);
            track->insertEventWhileRetainingSort(noteEvent);
        }

        sequencerPlaybackEngine->enqueueEventAfterNFrames(
            [voices = voices, note, tickPosition, drumBus]
            {
                if (drumBus)
                {
                    const auto ctx =
                        DrumNoteEventContextBuilder::buildDrumNoteOffContext(
                            0, drumBus, voices, note, tickPosition);
                    DrumNoteEventHandler::noteOff(ctx);
                }
            },
            durationFrames - 1);
    }
}
