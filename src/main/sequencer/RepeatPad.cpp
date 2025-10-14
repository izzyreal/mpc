#include "RepeatPad.hpp"

#include "Mpc.hpp"
#include "Util.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/MidiOutput.hpp"
#include "hardware2/Hardware2.h"
#include "lcdgui/screens/window/Assign16LevelsScreen.hpp"
#include "sequencer/FrameSeq.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sequencer/Track.hpp"

using namespace mpc::sequencer;
using namespace mpc::lcdgui::screens::window;

void RepeatPad::process(mpc::Mpc& mpc,
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

    auto sequencer = mpc.getSequencer();
    auto track = sequencer->getActiveTrack();

    std::shared_ptr<mpc::sampler::Program> program;

    if (track->getBus() > 0)
    {
        program = mpc.getSampler()->getProgram(mpc.getDrum(track->getBus() - 1).getProgram());
    }

    auto fullLevel = mpc.isFullLevelEnabled();
    auto sixteenLevels = mpc.isSixteenLevelsEnabled();

    auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>("assign-16-levels");
    auto note = assign16LevelsScreen->getNote();

    for (int padIndexWithBank = 0; padIndexWithBank < 64; ++padIndexWithBank)
    {
        if (!program->isPadRegisteredAsPressed(padIndexWithBank))
        {
            continue;
        }

        if (!sixteenLevels && program)
        {
            note = program->getNoteFromPad(padIndexWithBank);
        }

        auto noteEvent = std::make_shared<NoteOnEvent>(note);
        noteEvent->setTick(static_cast<int>(tickPosition));
        const bool isSliderNote = program && program->getSlider()->getNote() == note;

        if (program)
        {
            auto hardwareSlider = mpc.getHardware2()->getSlider();
            auto programSlider = program->getSlider();

            Util::SliderNoteVariationContext sliderNoteVariationContext {
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
                programSlider->getFilterHighRange()
            };

            mpc::Util::setSliderNoteVariationParameters(sliderNoteVariationContext, noteEvent);
        }

        const int hardwarePadIndex = padIndexWithBank % 16;
        const auto hardwarePad = mpc.getHardware2()->getPad(hardwarePadIndex);

        if (sixteenLevels)
        {
            Util::SixteenLevelsContext sixteenLevelsContext {
                sixteenLevels,
                assign16LevelsScreen->getType(),
                assign16LevelsScreen->getOriginalKeyPad(),
                assign16LevelsScreen->getNote(),
                assign16LevelsScreen->getParameter(),
                hardwarePadIndex
            };

            noteEvent->setVelocity(127);
            mpc::Util::set16LevelsValues(sixteenLevelsContext, noteEvent);
            note = noteEvent->getNote();
        }
        else
        {
            assert(hardwarePad->getPressure().has_value() || hardwarePad->getVelocity().has_value());
            const int velocityToUseIfNotFullLevel = hardwarePad->getPressure().value_or(hardwarePad->getVelocity().value());
            noteEvent->setVelocity(fullLevel ? 127 : velocityToUseIfNotFullLevel);
        }

        noteEvent->setDuration(durationTicks);

        noteEvent->getNoteOff()->setTick(static_cast<int>(tickPosition) + durationTicks);

        const auto velocityBeforeTrackVelocityRatioApplied = noteEvent->getVelocity();

        noteEvent->setVelocity(noteEvent->getVelocity() * (track->getVelocityRatio() * 0.01));

        const auto durationFrames = static_cast<int>(durationTicks == -1 ? -1 :
                                               SeqUtil::ticksToFrames(durationTicks, tempo, sampleRate));

        if (track->getBus() > 0 && note != 34)
        {
            const auto noteParameters = program->getNoteParameters(note);
            const auto sound = mpc.getSampler()->getSound(noteParameters->getSoundIndex());
            auto voiceOverlap = (sound && sound->isLoopEnabled()) ? 2 : noteParameters->getVoiceOverlap();

            mpc.getDrum(track->getBus() - 1).mpcNoteOn(
                note, noteEvent->getVelocity(), noteEvent->getVariationType(), noteEvent->getVariationValue(), eventFrameOffset, true, -1,
                voiceOverlap == 2 ? durationFrames : -1);
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
                [&mpc, track, note, noteEvent, tickPosition, sixteenLevels, program, padIndexWithBank]
                (const int bufferOffset){
            if (track->getBus() > 0)
            {
                mpc.getDrum(track->getBus() - 1).mpcNoteOff(note, bufferOffset, tickPosition);
                // Implement ghost pad glow
                //const auto padToNotify = sixteenLevels ? hardware->getPad(program->getPadIndexFromNote(note)) : padIndexWithBank;
                // Notify the pad of the note off/pad release
            }

            if (track->getDeviceIndex() > 0)
            {
                const auto noteOffMsg = noteEvent->getNoteOff()->createShortMessage((track->getDeviceIndex() - 1) % 16);
                noteOffMsg->bufferPos = bufferOffset;
                mpc.getMidiOutput()->enqueueMessageOutputA(noteOffMsg);
            }
        }, durationFrames - 1);

        //const auto padToNotify = sixteenLevels ? hardware->getPad(program->getPadIndexFromNote(note)) : padIndexWithBank;
        // Notify the pad of the note on/pad push, with the right velocity
    }
}

