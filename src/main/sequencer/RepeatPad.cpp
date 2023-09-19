#include "RepeatPad.hpp"

#include "Mpc.hpp"
#include "Util.hpp"
#include "audiomidi/AudioMidiServices.hpp"
#include "audiomidi/MidiOutput.hpp"
#include "hardware/Hardware.hpp"
#include "hardware/HwPad.hpp"
#include "hardware/TopPanel.hpp"
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

    auto hardware = mpc.getHardware();
    auto fullLevel = hardware->getTopPanel()->isFullLevelEnabled();
    auto sixteenLevels = hardware->getTopPanel()->isSixteenLevelsEnabled();

    auto assign16LevelsScreen = mpc.screens->get<Assign16LevelsScreen>("assign-16-levels");
    auto note = assign16LevelsScreen->getNote();

    for (auto& p : hardware->getPads())
    {
        if (!p->isPressed()) continue;

        if (!sixteenLevels && program)
        {
            auto padIndex = p->getPadIndexWithBankWhenLastPressed();
            note = program->getNoteFromPad(padIndex);
        }

        auto noteEvent = std::make_shared<NoteOnEvent>(note);
        noteEvent->setTick(static_cast<int>(tickPosition));

        if (program)
        {
            mpc::Util::setSliderNoteVariationParameters(mpc, noteEvent, program);
        }

        if (sixteenLevels)
        {
            noteEvent->setVelocity(127);
            mpc::Util::set16LevelsValues(mpc, noteEvent, p->getIndex());
            note = noteEvent->getNote();
        }
        else
        {
            noteEvent->setVelocity(fullLevel ? 127 : p->getPressure());
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
                [&mpc, track, note, noteEvent, tickPosition, sixteenLevels, hardware, program, p]
                (const int bufferOffset){
            if (track->getBus() > 0)
            {
                mpc.getDrum(track->getBus() - 1).mpcNoteOff(note, bufferOffset, tickPosition);
                const auto padToNotify = sixteenLevels ? hardware->getPad(program->getPadIndexFromNote(note) % 16) : p;

                padToNotify->notifyObservers(255);
            }

            if (track->getDeviceIndex() > 0)
            {
                const auto noteOffMsg = noteEvent->getNoteOff()->createShortMessage((track->getDeviceIndex() - 1) % 16);
                noteOffMsg->bufferPos = bufferOffset;
                mpc.getMidiOutput()->enqueueMessageOutputA(noteOffMsg);
            }
        }, durationFrames - 1);

        const auto padToNotify = sixteenLevels ? hardware->getPad(program->getPadIndexFromNote(note) % 16) : p;

        padToNotify->notifyObservers(noteEvent->getVelocity());
    }
}
