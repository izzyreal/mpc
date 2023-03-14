#include "Drum.hpp"

#include <lcdgui/screens/MixerSetupScreen.hpp>

#include "FaderControl.hpp"
#include "MixerInterconnection.hpp"
#include "Voice.hpp"

#include "StereoMixer.hpp"
#include "IndivFxMixer.hpp"
#include <sampler/Sound.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/Program.hpp>
#include <sampler/NoteParameters.hpp>
#include "engine/audio/mixer/AudioMixerStrip.hpp"
#include "engine/audio/mixer/MainMixControls.hpp"
#include "engine/audio/mixer/PanControl.hpp"
#include "engine/control/CompoundControl.hpp"
#include <utility>

using namespace mpc::lcdgui::screens;
using namespace mpc::sampler;
using namespace mpc::engine;
using namespace mpc::engine::audio::mixer;
using namespace mpc::engine::audio::server;

Drum::Drum(std::shared_ptr<Sampler> samplerToUse,
           int drumIndexToUse,
           std::shared_ptr<AudioMixer> mixerToUse,
           const std::shared_ptr<AudioServer>& serverToUse,
           MixerSetupScreen* mixerSetupScreenToUse,
           std::vector<std::shared_ptr<Voice>> voicesToUse)
           : sampler(std::move(samplerToUse)), mixer(std::move(mixerToUse)),
           server(serverToUse.get()),  mixerSetupScreen(mixerSetupScreenToUse),
           voices(std::move(voicesToUse)), drumIndex(drumIndexToUse)

{
	receivePgmChange = true;
	receiveMidiVolume = true;

	for (int i = 0; i < 64; i++)
	{
		stereoMixerChannels.push_back(std::make_shared<StereoMixer>());
        weakStereoMixerChannels.push_back(stereoMixerChannels.back());

        indivFxMixerChannels.push_back(std::make_shared<IndivFxMixer>());
        weakIndivFxMixerChannels.push_back(indivFxMixerChannels.back());
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

void Drum::mpcNoteOn(int note, int velo, int varType, int varValue, int frameOffset, bool firstGeneration, int startTick, int durationFrames)
{
	if (note < 35 || note > 98 || velo == 0)
		return;

	auto program = sampler->getProgram(programNumber);
	auto np = program->getNoteParameters(note);

	checkForMutes(np);
	auto soundNumber = np->getSoundIndex();

	std::shared_ptr<Voice> voice;

	for (auto& v : voices)
	{
		if (v->isFinished())
		{
			voice = v;
			break;
		}
	}

	if (soundNumber == -1 || !voice)
		return;

	auto sound = sampler->getSound(soundNumber);

	auto smc = program->getStereoMixerChannel(note - 35);
	auto ifmc = program->getIndivFxMixerChannel(note - 35);

	bool sSrcDrum = mixerSetupScreen->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	if (sSrcDrum)
		smc = stereoMixerChannels[note - 35];

	if (iSrcDrum)
		ifmc = indivFxMixerChannels[note - 35];

    auto mixerControls = mixer->getMixerControls();

	auto sc = mixerControls->getStripControls(std::to_string(voice->getStripNumber()));

	auto mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));
	std::dynamic_pointer_cast<PanControl>(mmc->find("Pan"))->setValue(static_cast<float>(smc->getPanning() / 100.0));
	std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"))->setValue(static_cast<float>(smc->getLevel()));

	sc = mixerControls->getStripControls(std::to_string(voice->getStripNumber() + 32));
	mmc = std::dynamic_pointer_cast<MainMixControls>(sc->find("Main"));

	//We make sure the voice strip duplicages that are used for mixing to ASSIGNABLE MIX OUT are not mixed into Main.
	auto faderControl = std::dynamic_pointer_cast<FaderControl>(mmc->find("Level"));
	if (faderControl->getValue() != 0) faderControl->setValue(0);

	if (ifmc->getOutput() > 0)
	{
		if (sound->isMono())
		{
			if (ifmc->getOutput() % 2 == 1)
			{
				mixerConnections[voice->getStripNumber() - 1]->setLeftEnabled(true);
				mixerConnections[voice->getStripNumber() - 1]->setRightEnabled(false);
			}
			else
			{
				mixerConnections[voice->getStripNumber() - 1]->setLeftEnabled(false);
				mixerConnections[voice->getStripNumber() - 1]->setRightEnabled(true);
			}
		}
		else
		{
			mixerConnections[voice->getStripNumber() - 1]->setLeftEnabled(true);
			mixerConnections[voice->getStripNumber() - 1]->setRightEnabled(true);
		}
	}

	/*
	* We iterate through AUX#1 - #4 (aka ASSIGNABLE MIX OUT 1/2, 3/4, 5/6 and 7/8).
	* We silence the aux buses that are not in use by this voice.
	* We set the level for the MIX OUT that is in use, if any.
	*/
	int selectedAssignableMixOutPair = static_cast<int>(ceil((ifmc->getOutput() - 2) * 0.5));

	for (int i = 0; i < 4; i++)
	{
		auto auxControl = std::dynamic_pointer_cast<mpc::engine::control::CompoundControl>(sc->find("AUX#" + std::to_string(i + 1)));
		auto auxLevel = std::dynamic_pointer_cast<FaderControl>(auxControl->find("Level"));

		if (i == selectedAssignableMixOutPair)
		{
			auto value = static_cast<float>(ifmc->getVolumeIndividualOut());

			if (value != auxLevel->getValue())
				auxLevel->setValue(value);
		}
		else
		{
			if (auxLevel->getValue() != 0)
				auxLevel->setValue(0);
		}
	}

    if (np->getVoiceOverlap() == VoiceOverlapMode::MONO)
    {
        stopMonoOrPolyVoiceWithSameNoteParameters(np, note);
    }

	voice->init(velo, sound, note, np, varType, varValue, note, drumIndex, frameOffset, true, startTick, durationFrames);

	if (firstGeneration)
	{
		if (np->getSoundGenerationMode() == 1)
		{
			int optA = np->getOptionalNoteA();
			int optB = np->getOptionalNoteB();

			if (optA != 34)
			{
				mpcNoteOn(optA, velo, varType, varValue, frameOffset, false, startTick, durationFrames);
				simultA[note] = optA;
			}

			if (optB != 34)
			{
				mpcNoteOn(optB, velo, varType, varValue, frameOffset, false, startTick, durationFrames);
				simultB[note] = optB;
			}
		}
	}
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

void Drum::connectVoices()
{
	for (auto j = 0; j < 32; j++)
	{
		auto ams1 = mixer->getStrip(std::to_string(j + 1));
		auto voice = voices[j];
		ams1->setInputProcess(voice);
		auto mi = new MixerInterconnection("con" + std::to_string(j), server);
		ams1->setDirectOutputProcess(mi->getInputProcess());
		auto ams2 = mixer->getStrip(std::to_string(j + 1 + 32));
		ams2->setInputProcess(mi->getOutputProcess());
		mixerConnections.push_back(mi);
	}
}

std::vector<std::shared_ptr<StereoMixer>>& Drum::getStereoMixerChannels()
{
    return weakStereoMixerChannels;
}

std::vector<std::shared_ptr<IndivFxMixer>>& Drum::getIndivFxMixerChannels()
{
	return weakIndivFxMixerChannels;
}

void Drum::mpcNoteOff(int note, int frameOffset, int noteOnStartTick)
{
	if (note < 35 || note > 98)
		return;

    startDecayForNote(note, frameOffset, noteOnStartTick);

	auto it = simultA.find(note);

	if (it != simultA.end())
	{
		startDecayForNote(simultA[note], frameOffset, noteOnStartTick);
		simultA.erase(it);
	}

	it = simultB.find(note);

	if (it != simultB.end())
	{
		startDecayForNote(simultB[note], frameOffset, noteOnStartTick);
		simultB.erase(it);
	}
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

Drum::~Drum() {
	for (auto& m : mixerConnections)
	{
		if (m != nullptr)
			delete m;
	}
}
