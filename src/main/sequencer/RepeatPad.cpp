#include "RepeatPad.hpp"

#include "Mpc.hpp"
#include "Util.hpp"
#include "hardware/Hardware.hpp"
#include "hardware/HwPad.hpp"
#include "hardware/TopPanel.hpp"
#include "sequencer/Sequencer.hpp"
#include "sequencer/Track.hpp"
#include "sequencer/MidiAdapter.hpp"
#include "sequencer/SeqUtil.hpp"
#include "sampler/Sampler.hpp"

#include "engine/Drum.hpp"

using namespace mpc::sequencer;

void RepeatPad::process(mpc::Mpc& mpc,
                        unsigned int tickPosition,
                        int durationTicks,
                        unsigned short eventFrameOffset,
                        double tempo,
                        float sampleRate)
{
    auto sequencer = mpc.getSequencer();
    auto track = sequencer->getActiveTrack();

    if (mpc.getLayeredScreen()->getCurrentScreenName() != "sequencer" || track->getBus() == 0)
    {
        return;
    }

    auto program = mpc.getSampler()->getProgram(mpc.getDrum(track->getBus() - 1).getProgram());
    auto hardware = mpc.getHardware();
    auto fullLevel = hardware->getTopPanel()->isFullLevelEnabled();

    for (auto& p : hardware->getPads())
    {
        if (!p->isPressed()) continue;

        auto padIndex = p->getPadIndexWithBankWhenLastPressed();

        auto note = program->getNoteFromPad(padIndex);

        if (note != 34)
        {
            auto noteEvent = std::make_shared<NoteOnEvent>(note);
            noteEvent->setTick(static_cast<int>(tickPosition));
            noteEvent->setNote(note);
            mpc::Util::setSliderNoteVariationParameters(mpc, noteEvent, program);
            noteEvent->setVelocity(fullLevel ? 127 : p->getPressure());
            noteEvent->setDuration(durationTicks);

            noteEvent->getNoteOff()->setTick(static_cast<int>(tickPosition) + durationTicks);

            auto newVelo = static_cast<int>(noteEvent->getVelocity() * (track->getVelocityRatio() * 0.01));

            auto durationFrames = static_cast<int>(durationTicks == -1 ? -1 :
                    SeqUtil::ticksToFrames(durationTicks, tempo, sampleRate));

            auto voiceOverlap = program->getNoteParameters(note)->getVoiceOverlap();

            mpc.getDrum(track->getBus() - 1).mpcNoteOff(note, 0, 0);
            mpc.getDrum(track->getBus() - 1).mpcNoteOn(
                note, newVelo, noteEvent->getVariationType(), noteEvent->getVariationValue(), eventFrameOffset, true, -1,
                voiceOverlap == 2 ? durationFrames : -1);

            p->notifyObservers(newVelo);

            if (sequencer->isRecordingOrOverdubbing())
            {
                track->insertEventWhileRetainingSort(noteEvent);
            }
        }
    }
}
