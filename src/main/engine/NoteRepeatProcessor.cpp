#include "engine/NoteRepeatProcessor.hpp"

#include "engine/EngineHost.hpp"
#include "engine/DrumNoteEventHandler.hpp"
#include "engine/DrumNoteEventContextBuilder.hpp"

#include "audiomidi/EventHandler.hpp"

#include "performance/PerformanceManager.hpp"
#include "hardware/Component.hpp"

#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "lcdgui/screens/MixerSetupScreen.hpp"

#include "sampler/Sampler.hpp"

#include "sequencer/Transport.hpp"
#include "sequencer/Bus.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/SequencerStateManager.hpp"
#include "sequencer/TrackStateView.hpp"

#include "Util.hpp"
#include "audio/mixer/AudioMixer.hpp"

using namespace mpc;
using namespace mpc::engine;
using namespace mpc::sequencer;
using namespace mpc::performance;
using namespace mpc::sampler;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::performance;
using namespace mpc::engine::audio::mixer;

NoteRepeatProcessor::NoteRepeatProcessor(
    audiomidi::EventHandler *eventHandler, std::weak_ptr<Sequencer> s,
    std::shared_ptr<Sampler> sa, std::shared_ptr<AudioMixer> m,
    std::shared_ptr<Assign16LevelsScreen> a,
    std::shared_ptr<MixerSetupScreen> ms,
    const std::weak_ptr<PerformanceManager> &performanceManager,
    std::shared_ptr<hardware::Slider> h, std::vector<std::shared_ptr<Voice>> *v,
    std::vector<MixerInterconnection *> &mi,
    const std::function<bool()> &isFullLevelEnabled,
    const std::function<bool()> &isSixteenLevelsEnabled)
    : eventHandler(eventHandler), sequencer(std::move(s)),
      sampler(std::move(sa)), mixer(std::move(m)),
      assign16LevelsScreen(std::move(a)), mixerSetupScreen(std::move(ms)),
      performanceManager(performanceManager), hardwareSlider(std::move(h)),
      voices(v), mixerConnections(mi), isFullLevelEnabled(isFullLevelEnabled),
      isSixteenLevelsEnabled(isSixteenLevelsEnabled)
{
}

void NoteRepeatProcessor::process(EngineHost *engineHost,
                                  unsigned int tickPosition, int durationTicks,
                                  const unsigned short eventFrameOffset,
                                  const double tempo,
                                  const float sampleRate) const
{
    auto lockedSequencer = sequencer.lock();
    auto track = lockedSequencer->getSelectedTrack();
    auto drumBus = lockedSequencer->getBus<DrumBus>(track->getBusType());

    std::shared_ptr<sampler::Program> program;

    if (drumBus)
    {
        program = sampler->getProgram(drumBus->getProgramIndex());
    }

    DrumNoteNumber noteNumber = assign16LevelsScreen->getNote();

    const auto snapshot = performanceManager.lock()->getSnapshot();

    static constexpr std::initializer_list<PerformanceEventSource>
        sourcesToExclude{PerformanceEventSource::NoteRepeat,
                         PerformanceEventSource::Sequence};

    for (int programPadIndex = 0;
         programPadIndex < Mpc2000XlSpecs::MAX_LAST_PROGRAM_INDEX;
         ++programPadIndex)
    {
        auto mostRecentProgramPadPressEvent =
            snapshot.getMostRecentProgramPadPress(
                ProgramPadIndex(programPadIndex), sourcesToExclude);

        if (!mostRecentProgramPadPressEvent)
        {
            continue;
        }

        if (!isSixteenLevelsEnabled() && program && program->isUsed())
        {
            noteNumber =
                program->getNoteFromPad(ProgramPadIndex(programPadIndex));
        }

        EventData noteEvent;
        noteEvent.type = EventType::NoteOn;
        noteEvent.noteNumber = noteNumber;
        noteEvent.tick = tickPosition;
        noteEvent.trackIndex = track->getIndex();

        const bool isSliderNote =
            program && program->getSlider()->getNote() == noteNumber;

        if (program && isSliderNote)
        {
            const auto programSlider = program->getSlider();

            Util::SliderNoteVariationContext sliderNoteVariationContext{
                hardwareSlider->getValueAs<int>(),
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
            noteEvent.noteVariationType = type;
            noteEvent.noteVariationValue = NoteVariationValue(value);
        }

        const PhysicalPadIndex physicalPadIndex =
            mostRecentProgramPadPressEvent->physicalPadIndex;

        if (const bool isPhysicallyPressed =
                physicalPadIndex != NoPhysicalPadIndex;
            isSixteenLevelsEnabled() && isPhysicallyPressed)
        {
            Util::SixteenLevelsContext sixteenLevelsContext{
                isSixteenLevelsEnabled(),
                assign16LevelsScreen->getType(),
                assign16LevelsScreen->getOriginalKeyPad(),
                assign16LevelsScreen->getNote(),
                assign16LevelsScreen->getParameter(),
                programPadIndex % 16};

            noteEvent.velocity = MaxVelocity;
            Util::set16LevelsValues(sixteenLevelsContext, noteEvent);
        }
        else
        {
            const Velocity velocityToUseIfNotFullLevel =
                snapshot.getPressedProgramPadAfterTouchOrVelocity(
                    ProgramPadIndex(programPadIndex));

            noteEvent.velocity = isFullLevelEnabled()
                                     ? MaxVelocity
                                     : velocityToUseIfNotFullLevel;
        }

        noteEvent.duration = Duration(durationTicks);

        const auto velocityBeforeTrackVelocityRatioApplied = noteEvent.velocity;

        noteEvent.velocity = Velocity(velocityBeforeTrackVelocityRatioApplied *
                                      (track->getVelocityRatio() * 0.01));

        const auto durationFrames = static_cast<int>(
            SeqUtil::ticksToFrames(durationTicks, tempo, sampleRate));

        if (drumBus && noteNumber != NoDrumNoteAssigned)
        {
            const auto noteParameters = program->getNoteParameters(noteNumber);
            const auto sound =
                sampler->getSound(noteParameters->getSoundIndex());
            const auto voiceOverlap =
                sound && sound->isLoopEnabled()
                    ? VoiceOverlapMode::NOTE_OFF
                    : noteParameters->getVoiceOverlapMode();

            auto performanceDrum =
                performanceManager.lock()->getSnapshot().getDrum(
                    drumBus->getIndex());

            auto ctx = DrumNoteEventContextBuilder::buildDrumNoteOnContext(
                0, performanceDrum, drumBus, sampler, mixer, mixerSetupScreen,
                voices, mixerConnections, noteNumber, noteEvent.velocity,
                noteEvent.noteVariationType, noteEvent.noteVariationValue,
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
            eventHandler->handleNoteEventMidiOut(
                noteEvent, track->getDeviceIndex(), std::nullopt, std::nullopt,
                eventFrameOffset);
        }

        if (lockedSequencer->getTransport()->isRecordingOrOverdubbing())
        {
            noteEvent.velocity = velocityBeforeTrackVelocityRatioApplied;

            auto existingNoteEvent =
                lockedSequencer->getStateManager()
                    ->getSnapshot()
                    .getTrackState(lockedSequencer->getSelectedSequenceIndex(),
                                   track->getIndex())
                    .findNoteEvent(noteEvent.tick, noteEvent.noteNumber);

            if (existingNoteEvent)
            {
                existingNoteEvent->copyNoteEventPropertiesFrom(&noteEvent);
            }
            else
            {
                track->acquireAndInsertEvent(noteEvent);
            }
        }

        concurrency::SamplePreciseTask task;

        task.f.set(
            [this, track, noteNumber, tickPosition,
             drumBus](const int frameOffset)
            {
                if (drumBus)
                {
                    const auto ctx =
                        DrumNoteEventContextBuilder::buildDrumNoteOffContext(
                            0, drumBus, voices, noteNumber, tickPosition);

                    DrumNoteEventHandler::noteOff(ctx);
                }

                if (track->getDeviceIndex() > 0)
                {
                    EventData noteOffEvent;
                    noteOffEvent.type = EventType::NoteOff;
                    noteOffEvent.noteNumber = noteNumber;
                    eventHandler->handleNoteEventMidiOut(
                        noteOffEvent, track->getDeviceIndex(), std::nullopt,
                        std::nullopt, frameOffset);
                }
            });

        const auto gapBetweenTwoAdjacentNoteEvents =
            ((sampleRate * 0.001) * 1.0);

        task.nFrames = durationFrames - gapBetweenTwoAdjacentNoteEvents;

        task.nFrames -=
            (mixer->getSharedBuffer()->getSampleCount() - eventFrameOffset);

        engineHost->postSamplePreciseTaskToAudioThread(task);
    }
}
