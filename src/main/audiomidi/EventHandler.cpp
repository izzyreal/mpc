#include "EventHandler.hpp"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/MpcMidiPorts.hpp>

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
#include <lcdgui/screens/window/CountMetronomeScreen.hpp>
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
    auto countMetronomeScreen = mpc.screens->get<CountMetronomeScreen>("count-metronome");

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
        // Currently unsure whether MASTER tempo mode supports tempo changes.
        // I suspect it does. This would mean Sequencer would need to house a
        // master tempo change meta track.  We'd need to route some get/set
        // tempo calls probably. Until further notice we only allow
        // tempo change events if the tempo source is SEQUENCE.
        if (sequencer->isTempoSourceSequenceEnabled())
            sequencer->setTempo(tce->getTempo());
        
        return;
    }
    else if (mce)
    {
        auto mpcMidiPorts = mpc.getMidiPorts();
        auto clockMsg = dynamic_cast<ctoot::midi::core::ShortMessage*>(mce->getShortMessage());
        clockMsg->setMessage(mce->getStatus());
        
        auto midiOutputStreamA = &mpcMidiPorts->getReceivers()[0];
        auto midiOutputStreamB = &mpcMidiPorts->getReceivers()[1];
        
        auto syncScreen = mpc.screens->get<SyncScreen>("sync");
        
        switch (syncScreen->out)
        {
            case 0:
                midiOutputStreamA->push_back(*clockMsg);
                break;
            case 1:
                midiOutputStreamB->push_back(*clockMsg);
                break;
            case 2:
                midiOutputStreamA->push_back(*clockMsg);
                midiOutputStreamB->push_back(*clockMsg);
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
                    auto eventFrame = mpc.getAudioMidiServices()->getFrameSequencer()->getEventFrameOffset();
                    
                    if (timeStamp != -1)
                        eventFrame = timeStamp;

                    int startTick = ne->getNoteOff().lock() ? ne->getTick() : ne->getNoteOnTick();

                    auto pgmIndex = sampler->getDrumBusProgramIndex(drumIndex + 1);
                    auto pgm = sampler->getProgram(pgmIndex);
                    auto voiceOverlap = pgm->getNoteParameters(ne->getNote())->getVoiceOverlap();
                    auto duration = voiceOverlap == 2 ? ne->getDuration() : -1;
                    auto audioServer = mpc.getAudioMidiServices()->getAudioServer();
                    auto durationFrames = (duration == -1 || duration == 0) ? -1 : mpc::sequencer::SeqUtil::ticksToFrames(duration, sequencer->getTempo(), audioServer->getSampleRate());

                    mpc.getMms()->mpcTransport(midiAdapter.get().lock().get(), 0, ne->getVariationType(), ne->getVariationValue(), eventFrame, startTick, durationFrames);
                    
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
                auto copy = std::make_shared<NoteEvent>(true);
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
        
        auto deviceNumber = track->getDevice() - 1;
        
        if (deviceNumber < 0)
            return;
        
        auto channel = deviceNumber;
        
        if (channel > 15)
            channel -= 16;
        
        MidiAdapter midiAdapter;
        midiAdapter.process(noteEvent, channel, -1);
        ctoot::midi::core::ShortMessage& msg = *midiAdapter.get().lock();
        
        auto mpcMidiPorts = mpc.getMidiPorts();
        
        std::vector<ctoot::midi::core::ShortMessage>& r = mpcMidiPorts->getReceivers()[0];
        
        std::string notifyLetter = "a";
        
        if (deviceNumber > 15)
        {
            deviceNumber -= 16;
            r = mpcMidiPorts->getReceivers()[1];
            notifyLetter = "b";
        }
        
        auto directToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
        
        if (!(mpc.getAudioMidiServices()->isBouncing() &&
              directToDiskRecorderScreen->offline) &&
            track->getDevice() != 0)
        {
            auto fs = mpc.getAudioMidiServices()->getFrameSequencer();
            auto eventFrame = fs->getEventFrameOffset();
            msg.bufferPos = eventFrame;
            
            if (r.size() < 100)
                r.push_back(msg);
            else
                r.clear();
        }
        
        notifyObservers(notifyLetter + std::to_string(deviceNumber));
    }
}
