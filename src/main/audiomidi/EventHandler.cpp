#include "EventHandler.hpp"

#include <Mpc.hpp>
#include <audiomidi/AudioMidiServices.hpp>
#include <audiomidi/MidiOutput.hpp>

#include <sequencer/Event.hpp>
#include <sequencer/FrameSeq.hpp>
#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>
#include <sequencer/SeqUtil.hpp>

#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/TransScreen.hpp>
#include <lcdgui/screens/SyncScreen.hpp>
#include <lcdgui/screens/window/VmpcDirectToDiskRecorderScreen.hpp>

#include <engine/midi/ShortMessage.hpp>

#include <engine/audio/server/NonRealTimeAudioServer.hpp>

#include <algorithm>

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

void EventHandler::handle(const std::shared_ptr<Event>& event, Track* track, std::optional<uint8_t> drum)
{
    if (!track->isOn() && event->getTick() != -1)
    {
        return;
    }

    handleNoThru(event, track, -1, drum);
    midiOut(event, track);
}

void EventHandler::handleNoThru(const std::shared_ptr<Event>& event, Track* track, int timeStamp, std::optional<uint8_t> clientDrumIndex)
{
    if (sequencer->isCountingIn() && event->getTick() != -1)
    {
        return;
    }
    
    auto mixerEvent = std::dynamic_pointer_cast<MixerEvent>(event);

    auto noteOnEvent = std::dynamic_pointer_cast<NoteOnEvent>(event);
    auto noteOffEvent = std::dynamic_pointer_cast<NoteOffEvent>(event);

    if (noteOnEvent || noteOffEvent)
    {
        if (auto drumIndex = clientDrumIndex.has_value() ? *clientDrumIndex : track->getBus() - 1;
            drumIndex >= 0)
        {
            handleDrumEvent(timeStamp, noteOnEvent, noteOffEvent, drumIndex, track);
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

        const auto audioMidiServices = mpc.getAudioMidiServices();
        const auto frameSeq = audioMidiServices->getFrameSequencer();
        const auto audioServer = audioMidiServices->getAudioServer();

        if (noteOnEvent->isFinalized())
        {
            frameSeq->enqueueEventAfterNFrames([this, noteOnEvent, track](int)
                {
                    midiOut(noteOnEvent->getNoteOff(), track);
                },
                SeqUtil::ticksToFrames(*noteOnEvent->getDuration(), sequencer->getTempo(), audioServer->getSampleRate())
            );
        }
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
}

void EventHandler::handleDrumEvent(int timeStamp, const std::shared_ptr<mpc::sequencer::NoteOnEvent>& noteOnEvent,
                                   const std::shared_ptr<mpc::sequencer::NoteOffEvent>& noteOffEvent, uint8_t drumIndex,
                                   mpc::sequencer::Track *track)
{
    const auto audioMidiServices = mpc.getAudioMidiServices();
    const auto frameSeq = audioMidiServices->getFrameSequencer();
    const auto audioServer = audioMidiServices->getAudioServer();

    auto eventFrame = timeStamp != -1 ? timeStamp : frameSeq->getEventFrameOffset();

    if (noteOffEvent && isDrumNote(noteOffEvent->getNote()))
    {
        mpc.getDrum(drumIndex).mpcNoteOff(noteOffEvent->getNote(), eventFrame,
                                          noteOffEvent->getTick());
    }
    else if (noteOnEvent && isDrumNote(noteOnEvent->getNote()) &&
               (noteOnEvent->isFinalized() || noteOnEvent->isPlayOnly()))
    {
        if (!sequencer->isSoloEnabled() || track->getIndex() == sequencer->getActiveTrackIndex())
        {
            const auto newVelo = std::clamp<int>(static_cast<int>(noteOnEvent->getVelocity() *
                                                                  (track->getVelocityRatio() *
                                                                   0.01)), 1, 127);
            const auto pgmIndex = sampler->getDrumBusProgramIndex(drumIndex + 1);
            const auto pgm = sampler->getProgram(pgmIndex);
            const auto noteParameters = pgm->getNoteParameters(noteOnEvent->getNote());

            const std::shared_ptr<mpc::sampler::Sound> sound = sampler->getSound(noteParameters->getSoundIndex());

            const auto voiceOverlap = (sound && sound->isLoopEnabled()) ? 2 : noteParameters->getVoiceOverlap();

            const auto duration = voiceOverlap == 2 ? noteOnEvent->getDuration() : NoteOnEvent::Duration();

            const auto durationFrames = (duration > 0) ?
                                        SeqUtil::ticksToFrames(*duration, sequencer->getTempo(),
                                                               audioServer->getSampleRate()) : -1;

            mpc.getDrum(drumIndex).mpcNoteOn(noteOnEvent->getNote(),
                                             newVelo,
                                             noteOnEvent->getVariationType(),
                                             noteOnEvent->getVariationValue(),
                                             eventFrame,
                                             true,
                                             noteOnEvent->getTick(),
                                             durationFrames);
        }
    }

    if (audioServer->isRealTime())
    {
        const auto note = noteOnEvent ? noteOnEvent->getNote() : noteOffEvent->getNote();
        const auto program = mpc.getSampler()->getProgram(mpc.getDrum(drumIndex).getProgram());
        int pad = program->getPadIndexFromNote(note);

        if (!mpc.isSixteenLevelsEnabled())
        {
            int bank = mpc.getBank();
            pad -= pad == -1 ? 0 : bank * 16;
        }

        bool visible(pad >= 0 && pad <= 15);

        if (visible)
        {
            if (noteOnEvent)
            {
                const auto padBrightness = static_cast<int>(noteOnEvent->getVelocity() *
                                                            (track->getVelocityRatio() * 0.01));
                //mpc.getHardware()->getPad(pad)->notifyObservers(padBrightness);
            }
            else
            {
                //mpc.getHardware()->getPad(pad)->notifyObservers(255);
            }
        }

        if (noteOnEvent && noteOnEvent->isFinalized())
        {
            frameSeq->enqueueEventAfterNFrames([this, noteOnEvent, visible, pad, track](int) {

                                                   if (visible)
                                                   {
                                                       //mpc.getHardware()->getPad(pad)->notifyObservers(255);
                                                   }
                                                   midiOut(noteOnEvent->getNoteOff(), track);
                                               },
                                               SeqUtil::ticksToFrames(*noteOnEvent->getDuration(),
                                                                      sequencer->getTempo(),
                                                                      audioServer->getSampleRate())
            );
        }
    }
}
