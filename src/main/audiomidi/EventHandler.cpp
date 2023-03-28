#include "EventHandler.hpp"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/MpcMidiOutput.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/FrameSeq.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>
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
    auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event);

    auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(event);
    auto noteOnEventPlayOnly = std::dynamic_pointer_cast<NoteOnEventPlayOnly>(event);
    auto noteOffEvent = std::dynamic_pointer_cast<NoteOffEvent>(event);
    
    if (noteOnEvent || noteOffEvent)
    {
        
        if (auto drumIndex = drum == -1 ? track->getBus() - 1 : drum; drumIndex >= 0)
        {
            auto frameSeq = mpc.getAudioMidiServices()->getFrameSequencer();
            auto eventFrame = timeStamp != -1 ? timeStamp : frameSeq->getEventFrameOffset();
            
            auto audioServer = mpc.getAudioMidiServices()->getAudioServer();
            
            if (noteOffEvent && noteOffEvent->isDrumNote())
            {
                mpc.getDrum(drumIndex).mpcNoteOff(noteOffEvent->getNote(), eventFrame, noteOffEvent->getTick());
            }
            else if (noteOnEvent->isDrumNote() && (noteOnEvent->isFinalized() || noteOnEventPlayOnly))
            {
                if (!sequencer->isSoloEnabled() || track->getIndex() == sequencer->getActiveTrackIndex())
                {
                    auto newVelo = static_cast<int>(noteOnEvent->getVelocity() * (track->getVelocityRatio() * 0.01));
                    auto pgmIndex = sampler->getDrumBusProgramIndex(drumIndex + 1);
                    auto pgm = sampler->getProgram(pgmIndex);
                    auto voiceOverlap = pgm->getNoteParameters(noteOnEvent->getNote())->getVoiceOverlap();
                    auto duration = voiceOverlap == 2 ? noteOnEvent->getDuration() : -1;///???????????
                    auto durationFrames = (duration == -1 || duration == 0) ?
                        -1 : SeqUtil::ticksToFrames(duration, sequencer->getTempo(), audioServer->getSampleRate());

                    mpc.getDrum(drumIndex).mpcNoteOn
                    (
                        noteOnEvent->getNote(),
                        newVelo,
                        noteOnEvent->getVariationType(),
                        noteOnEvent->getVariationValue(),
                        eventFrame,
                        true,
                        noteOnEvent->getTick(),
                        durationFrames
                    );
                }
            }
            if (audioServer->isRealTime())
            {
                auto note = noteOnEvent ? noteOnEvent->getNote() : noteOffEvent->getNote();
                int pad = mpc.getSampler()->getProgram(mpc.getDrum(drumIndex).getProgram())->getPadIndexFromNote(note);
                if (pad != -1) pad -= mpc.getBank() * 16;
                bool visible(pad >= 0 && pad <= 15);

                if (visible)
                {
                    if (noteOnEvent)
                        mpc.getHardware()->getPad(pad)->notifyObservers(static_cast<int>(noteOnEvent->getVelocity() * (track->getVelocityRatio() * 0.01)));
                    else
                        mpc.getHardware()->getPad(pad)->notifyObservers(255);
                }
                if (noteOnEvent && noteOnEvent->isFinalized())
                {
                    frameSeq->enqueueEventAfterNFrames([this, noteOnEvent, visible, pad, track](int)
                        {
                            if (visible) mpc.getHardware()->getPad(pad)->notifyObservers(255);
                            midiOut(noteOnEvent->getNoteOff(), track);
                        }, 
                        SeqUtil::ticksToFrames(noteOnEvent->getDuration(), sequencer->getTempo(), audioServer->getSampleRate())
                    );
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
    else if (tempoChangeEvent && tempoChangeEvent->getTempo() != sequencer->getTempo())
    {
        sequencer->setTempo(tempoChangeEvent->getTempo());
    }
}

void EventHandler::midiOut(const std::shared_ptr<Event>& e, Track* track)
{
    int transposeAmount = 0;

    int deviceNumber = track->getDeviceIndex() - 1; if (deviceNumber < 0) return;
    int channel = deviceNumber % 16;

    auto directToDiskRecorderScreen = mpc.screens->get<VmpcDirectToDiskRecorderScreen>("vmpc-direct-to-disk-recorder");
        
    std::shared_ptr<mpc::engine::midi::ShortMessage> msg;

    if (auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(e))
    {
        auto transScreen = mpc.screens->get<TransScreen>("trans");
        if (transScreen->transposeAmount != 0 && track->getIndex() < 64 && (transScreen->tr == -1 || transScreen->tr == track->getIndex()))
        {
            transposeAmount = transScreen->transposeAmount;
            transposeCache[noteOnEvent->getNoteOff()] = transposeAmount;
        }
        msg = noteOnEvent->createShortMessage(channel, transposeAmount);
    }
    else if (auto noteOffEvent = std::dynamic_pointer_cast<NoteOffEvent>(e))
    {
        auto candidate = transposeCache.find(noteOffEvent);
        if (candidate != end(transposeCache) && track->getIndex() < 64)
        {
            transposeAmount = candidate->second;
            transposeCache.erase(candidate);
        }
        msg = noteOffEvent->createShortMessage(channel, transposeAmount);
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

