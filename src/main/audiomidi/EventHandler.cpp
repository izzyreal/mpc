#include "EventHandler.hpp"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/MpcMidiOutput.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/FrameSeq.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/NoteEvent.hpp>
#include <sequencer/TempoChangeEvent.hpp>
#include <sequencer/SeqUtil.hpp>

#include <hardware/Hardware.hpp>
#include <hardware/HwPad.hpp>

#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/TransScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>

#include <midi/core/MidiMessage.hpp>
#include <midi/core/ShortMessage.hpp>
#include <midi/core/MidiInput.hpp>

#include <audio/server/NonRealTimeAudioServer.hpp>

#include <mpc/MpcMultiMidiSynth.hpp>
#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;
using namespace mpc::audiomidi;
using namespace mpc::sequencer;

EventHandler::EventHandler(mpc::Mpc& _mpc)
: mpc (_mpc),
sequencer (_mpc.getSequencer()),
sampler (_mpc.getSampler())
{
}

void EventHandler::handle(const std::shared_ptr<Event>& event, Track* track, char drum)
{
    if (!track->isOn() && event->getTick() != -1)
        return;
    
    auto ne = std::dynamic_pointer_cast<NoteEvent>(event);
    
    handleNoThru(event, track, -1, drum);
    midiOut(event, track);
}

void EventHandler::handleNoThru(const std::shared_ptr<Event>& event, Track* track, int timeStamp, char drum)
{
    if (sequencer->isCountingIn() && event->getTick() != -1)
    {
        return;
    }
    
    auto tempoChangeEvent = std::dynamic_pointer_cast<TempoChangeEvent>(event);
    auto noteEvent = std::dynamic_pointer_cast<NoteEvent>(event);
    auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event);
    
    if (tempoChangeEvent && tempoChangeEvent->getTempo() != sequencer->getTempo())
    {
        sequencer->setTempo(tempoChangeEvent->getTempo());
    }
    else if (noteEvent)
    {
        auto drumIndex = drum == -1 ? track->getBus() - 1 : drum;

        if (drumIndex >= 0)
        {
            if (noteEvent->getDuration() != -1)
            {
                auto isSolo = sequencer->isSoloEnabled();
                auto eventTrackIsSoloTrack = track->getIndex() == sequencer->getActiveTrackIndex();

                if (!isSolo || eventTrackIsSoloTrack || noteEvent->getVelocity() == 0)
                {
                    auto newVelo = static_cast<int>(noteEvent->getVelocity() * (track->getVelocityRatio() * 0.01));
                    MidiAdapter midiAdapter;
                    // Each of the 4 DRUMs is routed to their respective 0-based MIDI channel.
                    // This MIDI channel is part of a MIDI system that is internal to VMPC2000XL's sound player engine.
                    midiAdapter.process(noteEvent, drumIndex, newVelo);

                    auto audioServer = mpc.getAudioMidiServices()->getAudioServer();
                    auto frameSeq = mpc.getAudioMidiServices()->getFrameSequencer();
                    auto isDrumNote = noteEvent->getNote() >= 35 && noteEvent->getNote() <= 98;

                    if (isDrumNote)
                    {
                        auto eventFrame = frameSeq->getEventFrameOffset();

                        if (timeStamp != -1)
                            eventFrame = timeStamp;

                        auto pgmIndex = sampler->getDrumBusProgramIndex(drumIndex + 1);
                        auto pgm = sampler->getProgram(pgmIndex);
                        auto voiceOverlap = pgm->getNoteParameters(noteEvent->getNote())->getVoiceOverlap();
                        auto duration = voiceOverlap == 2 ? noteEvent->getDuration() : -1;
                        auto durationFrames = (duration == -1 || duration == 0) ?
                                              -1 : SeqUtil::ticksToFrames(duration, sequencer->getTempo(), audioServer->getSampleRate());

                        mpc.getMms()->mpcTransport(
                                midiAdapter.get().lock().get(),
                                0,
                                noteEvent->getVariationType(),
                                noteEvent->getVariationValue(),
                                eventFrame,
                                noteEvent->getTick(),
                                static_cast<int>(durationFrames));
                    }

                    if (audioServer->isRealTime())
                    {
                        auto note = noteEvent->getNote();
                        auto program = mpc.getSampler()->getProgram(mpc.getDrum(drumIndex)->getProgram());

                        int pad = program->getPadIndexFromNote(note);
                        int bank = mpc.getBank();

                        pad -= pad == -1 ? 0 : bank * 16;

                        if (pad >= 0 && pad <= 15)
                        {
                            int notifyVelo = noteEvent->getVelocity();

                            if (notifyVelo == 0)
                                notifyVelo = 255;

                            mpc.getHardware()->getPad(pad)->notifyObservers(notifyVelo);
                        }

                        if (noteEvent->getDuration() > 0)
                        {
                            const auto nFrames = SeqUtil::ticksToFrames(noteEvent->getDuration(), sequencer->getTempo(), audioServer->getSampleRate());
                            frameSeq->enqueueEventAfterNFrames([this, pad, noteEvent, track](int /*bufOffset*/) {
                                if (pad >= 0 && pad <= 15)
                                {
                                    mpc.getHardware()->getPad(pad)->notifyObservers(255);
                                }
                                midiOut(noteEvent->getNoteOff(), track);
                            }, static_cast<unsigned long>(nFrames));
                        }
                    }
                }
            }
        }
    }
    else if (mixerEvent)
    {
        auto pad = mixerEvent->getPad();
        auto p = sampler->getProgram(sampler->getDrumBusProgramIndex(track->getBus()));
        auto mixer = p->getStereoMixerChannel(pad).lock();
        
        auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
        
        if (mixerSetupScreen->isStereoMixSourceDrum())
        {
            auto busNumber = track->getBus();
            
            if (busNumber != 0)
            {
                auto drumIndex = busNumber - 1;
                auto mpcSoundPlayerChannel = mpc.getDrum(drumIndex);
                mixer = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
            }
            else
            {
                return;
            }
        }
        
        if (mixerEvent->getParameter() == 0)
            mixer->setLevel(mixerEvent->getValue());
        else if (mixerEvent->getParameter() == 1)
            mixer->setPanning(mixerEvent->getValue());
    }
}

void EventHandler::midiOut(const std::shared_ptr<Event>& e, Track* track)
{
    auto noteEvent = std::dynamic_pointer_cast<NoteEvent>(e);
    
    if (noteEvent)
    {
        if (noteEvent->getVelocity() == 0 && track->getIndex() < 64)
        {
            auto candidate = transposeCache.find({noteEvent->getNote(), track->getIndex() });
            
            if (candidate != end(transposeCache))
            {
                auto transposeParameters = *candidate;
                auto copy = std::make_shared<NoteEvent>(true, 0);
                noteEvent->CopyValuesTo(copy);
                noteEvent = copy;
                noteEvent->setNote(noteEvent->getNote() + transposeParameters.second);
                transposeCache.erase(candidate);
            }
        }
        
        auto transScreen = mpc.screens->get<TransScreen>("trans");
        
        if (track->getIndex() < 64 && transScreen->transposeAmount != 0 &&
            (transScreen->tr == -1 || transScreen->tr == noteEvent->getTrack()) &&
            noteEvent->getVelocity() > 0)
        {
            auto copy = std::make_shared<NoteEvent>();
            noteEvent->CopyValuesTo(copy);
            noteEvent = copy;
            transposeCache[{ noteEvent->getNote(), track->getIndex() }] = transScreen->transposeAmount;
            noteEvent->setNote(noteEvent->getNote() + transScreen->transposeAmount);
        }
        
        auto deviceNumber = track->getDeviceIndex() - 1;
        
        if (deviceNumber < 0)
            return;
        
        auto channel = deviceNumber;
        
        if (channel > 15)
            channel -= 16;
        
        MidiAdapter midiAdapter;
        midiAdapter.process(noteEvent, channel, -1);
        auto msg = midiAdapter.get().lock();
        
        auto mpcMidiOutput = mpc.getMidiOutput();

        std::string notifyLetter = "a";
        
        if (deviceNumber > 15)
        {
            deviceNumber -= 16;
            notifyLetter = "b";
        }

        auto directToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
        
        if (!(mpc.getAudioMidiServices()->isBouncing() &&
              directToDiskRecorderScreen->offline) &&
                track->getDeviceIndex() != 0)
        {
            auto fs = mpc.getAudioMidiServices()->getFrameSequencer();
            auto eventFrame = fs->getEventFrameOffset();
            msg->bufferPos = eventFrame;
            
            if (deviceNumber < 16)
            {
                mpcMidiOutput->enqueueMessageOutputA(msg);
            }
            else
            {
                mpcMidiOutput->enqueueMessageOutputB(msg);
            }
        }
        
        notifyObservers(notifyLetter + std::to_string(deviceNumber));
    }
}
