#include "Drum.hpp"

#include "Voice.hpp"

#include "StereoMixer.hpp"
#include "IndivFxMixer.hpp"
#include <sampler/Sound.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/NoteParameters.hpp>

using namespace mpc::engine;
using namespace mpc::sampler;

Drum::Drum(std::shared_ptr<Sampler> samplerToUse,
           const int drumIndexToUse,
           std::vector<std::shared_ptr<Voice>> &voicesToUse)
    : sampler(samplerToUse), 
    voices(voicesToUse),
    drumIndex(drumIndexToUse)
{
    receivePgmChange = true;
    receiveMidiVolume = true;

    for (int i = 0; i < 64; i++)
    {
        stereoMixerChannels.emplace_back(std::make_shared<StereoMixer>());
        indivFxMixerChannels.emplace_back(std::make_shared<IndivFxMixer>());
    }
}

void Drum::setProgram(int i)
{
    if (i < 0)
        return;

    if (!sampler->getProgram(i))
        return;

    programNumber = i;
}

int Drum::getProgram()
{
    return programNumber;
}

bool Drum::receivesPgmChange()
{
    return receivePgmChange;
}

void Drum::setReceivePgmChange(bool b)
{
    receivePgmChange = b;
}

bool Drum::receivesMidiVolume()
{
    return receiveMidiVolume;
}

void Drum::setReceiveMidiVolume(bool b)
{
    receiveMidiVolume = b;
}

int Drum::getLastReceivedMidiVolume()
{
    return lastReceivedMidiVolume;
}

void Drum::setLastReceivedMidiVolume(int volume)
{
    if (volume < 0 || volume > 127 || lastReceivedMidiVolume == volume)
        return;

    lastReceivedMidiVolume = volume;
}

void Drum::checkForMutes(NoteParameters* np)
{
    if (np->getMuteAssignA() != 34 || np->getMuteAssignB() != 34)
    {
        for (auto& v : voices)
        {
            if (v->isFinished())
                continue;

            if (v->getMuteInfo().shouldMute(np->getMuteAssignA(), drumIndex)
                    || v->getMuteInfo().shouldMute(np->getMuteAssignB(), drumIndex))
            {
                v->startDecay();
            }
        }
    }
}

void Drum::allNotesOff()
{
    for (int note = 35; note <= 98; note++)
    {
        for (auto& voice : voices)
        {
            if (!voice->isFinished()
                    && voice->getNote() == note
                    && voice->getVoiceOverlap() == VoiceOverlapMode::NOTE_OFF
                    && !voice->isDecaying()
                    && drumIndex == voice->getMuteInfo().getDrum())
            {
                voice->startDecay(0);
                break;
            }
        }
    }
}

void Drum::allSoundOff(int frameOffset)
{
    for (auto& voice : voices)
    {
        if (voice->isFinished())
            continue;

        voice->startDecay(frameOffset);
    }
}

std::vector<std::shared_ptr<StereoMixer>>& Drum::getStereoMixerChannels()
{
    return stereoMixerChannels;
}

std::vector<std::shared_ptr<IndivFxMixer>>& Drum::getIndivFxMixerChannels()
{
    return indivFxMixerChannels;
}

void Drum::startDecayForNote(const int note, const int frameOffset, const int noteOnStartTick)
{
    for (auto& voice : voices)
    {
        if (!voice->isFinished()
                && voice->getStartTick() == noteOnStartTick
                && voice->getNote() == note
                && voice->getVoiceOverlap() == VoiceOverlapMode::NOTE_OFF
                && !voice->isDecaying()
                && drumIndex == voice->getMuteInfo().getDrum())
        {
            voice->startDecay(frameOffset);
            break;
        }
    }
}

void Drum::stopMonoOrPolyVoiceWithSameNoteParameters(
        NoteParameters* noteParameters, int note)
{
    for (auto& voice : voices)
    {
        if (voice->getNoteParameters() == noteParameters) {
            if (voice->getNote() == note) {
                if (voice->getVoiceOverlap() == VoiceOverlapMode::POLY ||
                        voice->getVoiceOverlap() == VoiceOverlapMode::MONO)
                {
                    voice->startDecay();
                }
            }
        }
    }
}

