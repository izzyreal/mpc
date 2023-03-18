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

#include <engine/midi/MidiMessage.hpp>
#include <engine/midi/ShortMessage.hpp>

#include <engine/audio/server/NonRealTimeAudioServer.hpp>

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
                    // Each of the 4 DRUMs is routed to their respective 0-based MIDI channel.
                    // This MIDI channel is part of a MIDI system that is internal to VMPC2000XL's sound player engine.
                    auto msg = noteEvent->createShortMessage(drumIndex);
                    
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

                        if (noteEvent->getVelocity() > 0)
                        {
                            mpc.getDrum(drumIndex).mpcNoteOn(
                                    noteEvent->getNote(),
                                    noteEvent->getVelocity(),
                                    noteEvent->getVariationType(),
                                    noteEvent->getVariationValue(),
                                    eventFrame,
                                    true,
                                    noteEvent->getTick(),
                                    durationFrames
                                    );
                        }
                        else
                        {
                            mpc.getDrum(drumIndex).mpcNoteOff(noteEvent->getNote(), eventFrame, noteEvent->getTick());
                        }
                    }

                    if (audioServer->isRealTime())
                    {
                        auto note = noteEvent->getNote();
                        auto program = mpc.getSampler()->getProgram(mpc.getDrum(drumIndex).getProgram());

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
        auto mixer = p->getStereoMixerChannel(pad);
        
        auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");
        
        if (mixerSetupScreen->isStereoMixSourceDrum())
        {
            auto busNumber = track->getBus();
            
            if (busNumber != 0)
            {
                auto drumIndex = busNumber - 1;
                auto& mpcSoundPlayerChannel = mpc.getDrum(drumIndex);
                mixer = mpcSoundPlayerChannel.getStereoMixerChannels().at(pad);
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

    if (!noteEvent)
    {
        int transposeAmount = 0;

        int deviceNumber = track->getDeviceIndex() - 1; if (deviceNumber < 0) return;
        int channel = deviceNumber % 16;

        auto directToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
        
        std::shared_ptr<mpc::engine::midi::ShortMessage> msg;

        if (track->getIndex() < 64)
        {
            if (auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(e))
            {
                auto transScreen = mpc.screens->get<TransScreen>("trans");
                if (transScreen->transposeAmount != 0 && (transScreen->tr == -1 || transScreen->tr == noteOnEvent->getTrack()))
                {
                    transposeAmount = transScreen->transposeAmount;
                    transposeCache[{ noteOnEvent->getNote(), track->getIndex() }] = transposeAmount;
                }
                msg = noteOnEvent->createShortMessage(channel, transposeAmount);
            }
            else if (auto noteOffEvent = std::dynamic_pointer_cast<NoteOffEvent>(e))
            {
                auto candidate = transposeCache.find({ noteOffEvent->getNote(), track->getIndex() });
                if (candidate != end(transposeCache))
                {
                    transposeAmount = candidate->second;
                    transposeCache.erase(candidate);
                }
                msg = noteOffEvent->createShortMessage(channel, transposeAmount);
            }
        }

        if (!(mpc.getAudioMidiServices()->isBouncing() && directToDiskRecorderScreen->offline) && track->getDeviceIndex() != 0)
        {
            msg->bufferPos = mpc.getAudioMidiServices()->getFrameSequencer()->getEventFrameOffset();

            if (deviceNumber < 16)
            {
                mpc.getMidiOutput()->enqueueMessageOutputA(msg);
            }
            else
            {
                mpc.getMidiOutput()->enqueueMessageOutputA(msg);
            }
        }
        notifyObservers(deviceNumber < 16 ? "a" : "b" + std::to_string(channel));
        return;
    }
    
//-----------------
    auto noteEvent = std::dynamic_pointer_cast<NoteEvent>(e);
    
    if (noteEvent)
    {
        int transposeAmount = 0;

        if (noteEvent->isNoteOff() && track->getIndex() < 64)
        {
            auto candidate = transposeCache.find({noteEvent->getNote(), track->getIndex() });
            if (candidate != end(transposeCache))
            {
                transposeAmount = candidate->second;
                transposeCache.erase(candidate);
            }
        }
        
        else if (noteEvent->isNoteOn() && track->getIndex() < 64)
        {
            auto transScreen = mpc.screens->get<TransScreen>("trans");
            if (transScreen->transposeAmount != 0 && (transScreen->tr == -1 || transScreen->tr == noteEvent->getTrack()))
            {
                transposeAmount = transScreen->transposeAmount;
                transposeCache[{ noteEvent->getNote(), track->getIndex() }] = transposeAmount;
            }
        }
        
        int deviceNumber = track->getDeviceIndex() - 1; if (deviceNumber < 0) return;
        int channel = deviceNumber%16;
        
        auto directToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
        
        if (!(mpc.getAudioMidiServices()->isBouncing() && directToDiskRecorderScreen->offline) && track->getDeviceIndex() != 0)
        {
            auto msg = noteEvent->createShortMessage(channel, transposeAmount);
            msg->bufferPos = mpc.getAudioMidiServices()->getFrameSequencer()->getEventFrameOffset();
            
            if (deviceNumber < 16)
            {
                mpc.getMidiOutput()->enqueueMessageOutputA(msg);
            }
            else
            {
                mpc.getMidiOutput()->enqueueMessageOutputB(msg);
            }
        }
        
        notifyObservers(deviceNumber < 16 ? "a" : "b" + std::to_string(channel));
    }
}
