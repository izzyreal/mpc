#include "Drum.hpp"

#include <lcdgui/screens/MixerSetupScreen.hpp>

#include "FaderControl.hpp"
#include "MixerInterconnection.hpp"
#include "Voice.hpp"

#include "StereoMixer.hpp"
#include "IndivFxMixer.hpp"
#include <sampler/Sound.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/NoteParameters.hpp>
#include "engine/audio/mixer/AudioMixerStrip.hpp"
#include "engine/audio/mixer/MainMixControls.hpp"
#include "engine/audio/mixer/PanControl.hpp"
#include "engine/control/CompoundControl.hpp"

#include "engine/NoteEventProcessor.hpp"

#include <utility>

using namespace mpc::lcdgui::screens;
using namespace mpc::sampler;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;

Drum::Drum(std::shared_ptr<Sampler> samplerToUse,
           int drumIndexToUse,
           std::shared_ptr<AudioMixer> mixerToUse,
           MixerSetupScreen* mixerSetupScreenToUse,
           std::vector<std::shared_ptr<Voice>> voicesToUse,
           std::vector<mpc::engine::MixerInterconnection*>& mixerConnectionsToUse)
           : sampler(std::move(samplerToUse)), mixer(std::move(mixerToUse)),
           mixerSetupScreen(mixerSetupScreenToUse),
           voices(std::move(voicesToUse)), drumIndex(drumIndexToUse),
           mixerConnections(mixerConnectionsToUse)

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

void Drum::mpcNoteOn(int note, int velo, int varType, int varValue,
                     int frameOffset, bool firstGeneration,
                     int startTick, int durationFrames)
{
    NoteEventProcessor::noteOn(
        *sampler,
        *mixer,
        mixerSetupScreen,
        voices,
        stereoMixerChannels,
        indivFxMixerChannels,
        mixerConnections,
        simultA,
        simultB,
        drumIndex,
        programNumber,
        note, velo, varType, varValue,
        frameOffset, firstGeneration, startTick, durationFrames
    );
}

void Drum::mpcNoteOff(int note, int frameOffset, int noteOnStartTick)
{
    NoteEventProcessor::noteOff(
        voices, simultA, simultB,
        drumIndex,
        note, frameOffset, noteOnStartTick
    );
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
