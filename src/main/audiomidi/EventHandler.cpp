#include "EventHandler.hpp"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/MpcMidiOutput.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/FrameSeq.hpp>
#include <sequencer/MidiClockEvent.hpp>
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
    
    auto tce = std::dynamic_pointer_cast<TempoChangeEvent>(event);
    auto mce = std::dynamic_pointer_cast<MidiClockEvent>(event);
    auto ne = std::dynamic_pointer_cast<NoteEvent>(event);
    auto me = std::dynamic_pointer_cast<MixerEvent>(event);
    
    if (tce && tce->getTempo() != sequencer->getTempo())
    {
        sequencer->setTempo(tce->getTempo());
        return;
    }
    else if (mce)
    {
        auto mpcMidiOutput = mpc.getMidiOutput();
        auto clockMsg = std::shared_ptr<ctoot::midi::core::ShortMessage>(mce->getShortMessage());
        clockMsg->setMessage(mce->getStatus());

        auto syncScreen = mpc.screens->get<SyncScreen>("sync");
        
        switch (syncScreen->out)
        {
            case 0:
                mpcMidiOutput->enqueueMessageOutputA(clockMsg);
                break;
            case 1:
                mpcMidiOutput->enqueueMessageOutputB(clockMsg);
                break;
            case 2:
                mpcMidiOutput->enqueueMessageOutputA(clockMsg);
                mpcMidiOutput->enqueueMessageOutputB(clockMsg);
                break;
        }
    }
    else if (ne)
    {
        auto drumIndex = drum == -1 ? track->getBus() - 1 : drum;

        if (drumIndex >= 0)
        {
            if (ne->getDuration() != -1)
            {
                auto isSolo = sequencer->isSoloEnabled();
                auto eventTrackIsSoloTrack = track->getIndex() == sequencer->getActiveTrackIndex();

                if (!isSolo || eventTrackIsSoloTrack || ne->getVelocity() == 0)
                {
                    auto newVelo = static_cast<int>(ne->getVelocity() * (track->getVelocityRatio() * 0.01));
                    MidiAdapter midiAdapter;
                    // Each of the 4 DRUMs is routed to their respective 0-based MIDI channel.
                    // This MIDI channel is part of a MIDI system that is internal to VMPC2000XL's sound player engine.
                    midiAdapter.process(ne, drumIndex, newVelo);

                    auto frameSeq = mpc.getAudioMidiServices()->getFrameSequencer();
                    auto eventFrame = frameSeq->getEventFrameOffset();
                    
                    if (timeStamp != -1)
                        eventFrame = timeStamp;

                    auto pgmIndex = sampler->getDrumBusProgramIndex(drumIndex + 1);
                    auto pgm = sampler->getProgram(pgmIndex);
                    auto voiceOverlap = pgm->getNoteParameters(ne->getNote())->getVoiceOverlap();
                    auto duration = voiceOverlap == 2 ? ne->getDuration() : -1;
                    auto audioServer = mpc.getAudioMidiServices()->getAudioServer();
                    auto durationFrames = (duration == -1 || duration == 0) ?
                            -1 : SeqUtil::ticksToFrames(duration, sequencer->getTempo(), audioServer->getSampleRate());

                    mpc.getMms()->mpcTransport(midiAdapter.get().lock().get(), 0, ne->getVariationType(), ne->getVariationValue(), eventFrame, ne->getTick(), durationFrames);
                    
                    if (audioServer->isRealTime())
                    {
                        auto note = ne->getNote();
                        auto program = mpc.getSampler()->getProgram(mpc.getDrum(drumIndex)->getProgram());
                        
                        int pad = program->getPadIndexFromNote(note);
                        int bank = mpc.getBank();
                        pad -= bank * 16;
                        
                        if (pad >= 0 && pad <= 15)
                        {
                            int notifyVelo = ne->getVelocity();
                            
                            if (notifyVelo == 0)
                                notifyVelo = 255;
                            
                            mpc.getHardware()->getPad(pad)->notifyObservers(notifyVelo);

                            if (ne->getDuration() > 0)
                            {
                                const auto nFrames = SeqUtil::ticksToFrames(ne->getDuration(), sequencer->getTempo(), audioServer->getSampleRate());
                                frameSeq->enqueueEventAfterNFrames([this, pad, ne, track]() {
                                    mpc.getHardware()->getPad(pad)->notifyObservers(255);
                                    midiOut(ne->getNoteOff(), track);
                                }, static_cast<unsigned long>(nFrames));
                            }
                        }
                    }
                }
            }
        }
    }
    else if (me)
    {
        auto pad = me->getPad();
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
        
        if (me->getParameter() == 0)
            mixer->setLevel(me->getValue());
        else if (me->getParameter() == 1)
            mixer->setPanning(me->getValue());
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
