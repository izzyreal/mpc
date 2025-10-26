#include "RepeatPad.hpp"

#include "Mpc.hpp"
#include "Util.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/MidiOutput.hpp"
#include "controller/ClientEventController.hpp"
#include "controller/ClientHardwareEventController.hpp"
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

void RepeatPad::process(mpc::Mpc &mpc,
                        unsigned int tickPosition,
                        int durationTicks,
                        unsigned short eventFrameOffset,
                        double tempo,
                        float sampleRate)
{
    if (mpc.getLayeredScreen()->getCurrentScreenName() != "sequencer")
    {
        return;
    }

    using PadPressSource = mpc::sampler::Program::PadPressSource;

    auto sequencer = mpc.getSequencer();
    auto track = sequencer->getActiveTrack();

    std::shared_ptr<mpc::sampler::Program> program;

    if (track->getBus() > 0)
    {
        program = mpc.getSampler()->getProgram(mpc.getDrum(track->getBus() - 1).getProgram());
    }

    const auto fullLevel = mpc.clientEventController->isFullLevelEnabled();
    const auto sixteenLevels = mpc.clientEventController->isSixteenLevelsEnabled();

    auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>();
    auto note = assign16LevelsScreen->getNote();

    for (int physicalPadIndex = 0; physicalPadIndex < 16; ++physicalPadIndex)
    {
        for (int bankIndex = 0; bankIndex < 4; ++bankIndex)
        {
            if (!mpc.clientEventController->clientHardwareEventController->isPhysicallyPressed(physicalPadIndex, mpc.clientEventController->getActiveBank()))
            {
                continue;
            }

            const int programPadIndex = physicalPadIndex + bankIndex * 16;

            if (!sixteenLevels && program)
            {
                note = program->getNoteFromPad(programPadIndex);
            }

            auto noteEvent = std::make_shared<NoteOnEvent>(note);
            noteEvent->setTick(static_cast<int>(tickPosition));
            const bool isSliderNote = program && program->getSlider()->getNote() == note;

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

                mpc::Util::setSliderNoteVariationParameters(sliderNoteVariationContext, noteEvent);
            }

            if (sixteenLevels)
            {
                Util::SixteenLevelsContext sixteenLevelsContext{
                    sixteenLevels,
                    assign16LevelsScreen->getType(),
                    assign16LevelsScreen->getOriginalKeyPad(),
                    assign16LevelsScreen->getNote(),
                    assign16LevelsScreen->getParameter(),
                    physicalPadIndex};

                noteEvent->setVelocity(127);
                mpc::Util::set16LevelsValues(sixteenLevelsContext, noteEvent);
                note = noteEvent->getNote();
            }
            else
            {
                const auto hardwarePad = mpc.getHardware()->getPad(physicalPadIndex);
                assert(hardwarePad->getPressure().has_value() || hardwarePad->getVelocity().has_value());
                const int velocityToUseIfNotFullLevel = hardwarePad->getPressure().value_or(hardwarePad->getVelocity().value());
                noteEvent->setVelocity(fullLevel ? 127 : velocityToUseIfNotFullLevel);
            }

            noteEvent->setDuration(durationTicks);

            noteEvent->getNoteOff()->setTick(static_cast<int>(tickPosition) + durationTicks);

            const auto velocityBeforeTrackVelocityRatioApplied = noteEvent->getVelocity();

            noteEvent->setVelocity(noteEvent->getVelocity() * (track->getVelocityRatio() * 0.01));

            const auto durationFrames = static_cast<int>(durationTicks == -1 ? -1 : SeqUtil::ticksToFrames(durationTicks, tempo, sampleRate));

            if (track->getBus() > 0 && note != 34)
            {
                const auto noteParameters = program->getNoteParameters(note);
                const auto sound = mpc.getSampler()->getSound(noteParameters->getSoundIndex());
                auto voiceOverlap = (sound && sound->isLoopEnabled()) ? VoiceOverlapMode::NOTE_OFF : noteParameters->getVoiceOverlapMode();

                auto &drum = mpc.getDrum(track->getBus() - 1);

                auto ctx = DrumNoteEventContextBuilder::buildNoteOn(
                    0,
                    &drum,
                    mpc.getSampler(),
                    mpc.getAudioMidiServices()->getMixer(),
                    mpc.screens->get<MixerSetupScreen>(),
                    &mpc.getAudioMidiServices()->getVoices(),
                    mpc.getAudioMidiServices()->getMixerConnections(),
                    note,
                    noteEvent->getVelocity(),
                    noteEvent->getVariationType(),
                    noteEvent->getVariationValue(),
                    eventFrameOffset,
                    /* firstGeneration */ true, // Always true for invokers that are not DrumNoteEventHandler::noteOn itself
                    /*tick*/ -1,
                    voiceOverlap == VoiceOverlapMode::NOTE_OFF ? durationFrames : -1);

                DrumNoteEventHandler::noteOn(ctx);

                program->registerPadPress(programPadIndex, PadPressSource::NON_PHYSICAL);
            }

            if (track->getDeviceIndex() > 0)
            {
                const auto noteOnMsg = noteEvent->createShortMessage((track->getDeviceIndex() - 1) % 16);
                noteOnMsg->bufferPos = eventFrameOffset;
                mpc.getMidiOutput()->enqueueMessageOutputA(noteOnMsg);
            }

            if (sequencer->isRecordingOrOverdubbing())
            {
                noteEvent->setVelocity(velocityBeforeTrackVelocityRatioApplied);
                track->insertEventWhileRetainingSort(noteEvent);
            }

            mpc.getAudioMidiServices()->getFrameSequencer()->enqueueEventAfterNFrames(
                [&mpc, track, note, noteEvent, tickPosition, program, programPadIndex]
                {
                    if (track->getBus() > 0)
                    {
                        auto &drum = mpc.getDrum(track->getBus() - 1);

                        auto ctx = DrumNoteEventContextBuilder::buildNoteOff(
                            0,
                            &drum,
                            &mpc.getAudioMidiServices()->getVoices(),
                            note,
                            tickPosition);

                        DrumNoteEventHandler::noteOff(ctx);

                        program->registerPadRelease(programPadIndex, PadPressSource::NON_PHYSICAL);
                    }

                    if (track->getDeviceIndex() > 0)
                    {
                        const auto noteOffMsg = noteEvent->getNoteOff()->createShortMessage((track->getDeviceIndex() - 1) % 16);
                        // noteOffMsg->bufferPos = bufferOffset;
                        mpc.getMidiOutput()->enqueueMessageOutputA(noteOffMsg);
                    }
                },
                durationFrames - 1);
        }
    }
}
