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

#include "mpc/MpcSoundPlayerChannel.hpp"
#include "mpc/MpcMultiMidiSynth.hpp"
#include "midi/core/ShortMessage.hpp"

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

    auto program = mpc.getSampler()->getProgram(mpc.getDrum(track->getBus() - 1)->getProgram());
    auto hardware = mpc.getHardware();
    auto fullLevel = hardware->getTopPanel()->isFullLevelEnabled();

    for (auto& p : hardware->getPads())
    {
        if (!p->isPressed()) continue;

        auto padIndex = p->getPadIndexWithBankWhenLastPressed();

        auto note = program->getNoteFromPad(padIndex);

        if (note != 34)
        {
            auto noteEvent = std::make_shared<NoteEvent>(note);
            noteEvent->setTick(static_cast<int>(tickPosition));
            noteEvent->setNote(note);
            mpc::Util::setSliderNoteVariationParameters(mpc, noteEvent, program);
            noteEvent->setVelocity(fullLevel ? 127 : p->getPressure());
            noteEvent->setDuration(durationTicks);

            noteEvent->getNoteOff()->setTick(static_cast<int>(tickPosition) + durationTicks);
            noteEvent->getNoteOff()->setVelocity(0);

            MidiAdapter midiAdapter;

            midiAdapter.process(noteEvent->getNoteOff(), track->getBus() - 1, 0);
            auto voiceOverlap = program->getNoteParameters(note)->getVoiceOverlap();
            mpc.getMms()->mpcTransport(midiAdapter.get().lock().get(), 0, 0, 0, eventFrameOffset, -1, -1);

            auto newVelo = static_cast<int>(noteEvent->getVelocity() * (track->getVelocityRatio() * 0.01));
            midiAdapter.process(noteEvent, track->getBus() - 1, newVelo);

            auto durationFrames = static_cast<int>(durationTicks == -1 ? -1 :
                    SeqUtil::ticksToFrames(durationTicks, tempo, sampleRate));

            mpc.getMms()->mpcTransport(
                    midiAdapter.get().lock().get(),
                    0,
                    noteEvent->getVariationType(),
                    noteEvent->getVariationValue(),
                    eventFrameOffset,
                    -1,
                    voiceOverlap == 2 ? durationFrames : -1);

            p->notifyObservers(newVelo);

            if (sequencer->isRecordingOrOverdubbing())
            {
                track->insertEventWhileRetainingSort(noteEvent);
            }
        }
    }
}
