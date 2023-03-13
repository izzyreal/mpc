#include "MpcSoundPlayerChannel.hpp"

// ctootextensions
#include "MpcSoundPlayerControls.hpp"
#include "MpcFaderControl.hpp"
#include "MpcMixerInterconnection.hpp"
#include "MpcVoice.hpp"

// mpc
#include <engine/mpc/MpcNoteParameters.hpp>
#include <engine/mpc/MpcProgram.hpp>
#include <engine/mpc/MpcSampler.hpp>
#include <engine/mpc/MpcStereoMixerChannel.hpp>
#include <engine/mpc/MpcIndivFxMixerChannel.hpp>
#include <engine/mpc/MpcMixerSetupGui.hpp>
#include <engine/mpc/MpcSound.hpp>

// ctoot
#include <engine/audio/mixer/AudioMixerStrip.hpp>
#include <engine/audio/mixer/MainMixControls.hpp>
#include <engine/audio/mixer/PanControl.hpp>
#include <engine/control/CompoundControl.hpp>

using namespace ctoot::mpc;
using namespace std;

MpcSoundPlayerChannel::MpcSoundPlayerChannel(shared_ptr<MpcSoundPlayerControls> controlsToUse)
{
	controls = controlsToUse;
    voices = controls->getVoices();
	receivePgmChange = true;
	receiveMidiVolume = true;

	drumIndex = controls->getDrumIndex();
	sampler = controls->getSampler();
	mixer = controls->getMixer();
	server = controls->getServer();

	for (int i = 0; i < 64; i++)
	{
		stereoMixerChannels.push_back(make_shared<MpcStereoMixerChannel>());
        weakStereoMixerChannels.push_back(stereoMixerChannels.back());

        indivFxMixerChannels.push_back(make_shared<MpcIndivFxMixerChannel>());
        weakIndivFxMixerChannels.push_back(indivFxMixerChannels.back());
	}
}

void MpcSoundPlayerChannel::setProgram(int i)
{
	if (i < 0)
        return;

    if (!sampler->getMpcProgram(i))
        return;

    programNumber = i;
}

int MpcSoundPlayerChannel::getProgram()
{
	return programNumber;
}

bool MpcSoundPlayerChannel::receivesPgmChange()
{
	return receivePgmChange;
}

void MpcSoundPlayerChannel::setReceivePgmChange(bool b)
{
	receivePgmChange = b;
}

bool MpcSoundPlayerChannel::receivesMidiVolume()
{
	return receiveMidiVolume;
}

void MpcSoundPlayerChannel::setReceiveMidiVolume(bool b)
{
	receiveMidiVolume = b;
}

int MpcSoundPlayerChannel::getLastReceivedMidiVolume()
{
	return lastReceivedMidiVolume;
}

void MpcSoundPlayerChannel::setLastReceivedMidiVolume(int volume)
{
	if (volume < 0 || volume > 127 || lastReceivedMidiVolume == volume)
		return;

	lastReceivedMidiVolume = volume;
}

void MpcSoundPlayerChannel::mpcNoteOn(int note, int velo, int varType, int varValue, int frameOffset, bool firstGeneration, int startTick, int durationFrames)
{
	if (note < 35 || note > 98 || velo == 0)
		return;

	auto program = sampler->getMpcProgram(programNumber);
	auto np = program->getNoteParameters(note);

	checkForMutes(np);
	auto soundNumber = np->getSoundIndex();

	shared_ptr<MpcVoice> voice;

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

	auto sound = sampler->getMpcSound(soundNumber);

	auto smc = program->getStereoMixerChannel(note - 35);
	auto ifmc = program->getIndivFxMixerChannel(note - 35);

	bool sSrcDrum = controls->getMixerSetupGui()->isStereoMixSourceDrum();
	bool iSrcDrum = controls->getMixerSetupGui()->isIndivFxSourceDrum();

	if (sSrcDrum)
		smc = stereoMixerChannels[note - 35];

	if (iSrcDrum)
		ifmc = indivFxMixerChannels[note - 35];

    auto mixerControls = mixer->getMixerControls();

	auto sc = mixerControls->getStripControls(to_string(voice->getStripNumber()));

	auto mmc = dynamic_pointer_cast<ctoot::audio::mixer::MainMixControls>(sc->find("Main"));
	dynamic_pointer_cast<ctoot::audio::mixer::PanControl>(mmc->find("Pan"))->setValue(static_cast<float>(smc->getPanning() / 100.0));
	dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level"))->setValue(static_cast<float>(smc->getLevel()));

	sc = mixerControls->getStripControls(to_string(voice->getStripNumber() + 32));
	mmc = dynamic_pointer_cast<ctoot::audio::mixer::MainMixControls>(sc->find("Main"));

	//We make sure the voice strip duplicages that are used for mixing to ASSIGNABLE MIX OUT are not mixed into Main.
	auto faderControl = dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level"));
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
		auto auxControl = dynamic_pointer_cast<ctoot::control::CompoundControl>(sc->find("AUX#" + to_string(i + 1)));
		auto auxLevel = dynamic_pointer_cast<MpcFaderControl>(auxControl->find("Level"));

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

void MpcSoundPlayerChannel::checkForMutes(ctoot::mpc::MpcNoteParameters* np)
{
	if (np->getMuteAssignA() != 34 || np->getMuteAssignB() != 34)
	{
		for (auto& v : controls->getVoices())
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

void MpcSoundPlayerChannel::allNotesOff()
{
    for (int note = 35; note <= 98; note++)
    {
        for (auto& voice : controls->getVoices())
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

void MpcSoundPlayerChannel::allSoundOff(int frameOffset)
{
	for (auto& voice : controls->getVoices())
	{
		if (voice->isFinished())
			continue;

		voice->startDecay(frameOffset);
	}
}

void MpcSoundPlayerChannel::connectVoices()
{
	for (auto j = 0; j < 32; j++)
	{
		auto ams1 = mixer->getStrip(to_string(j + 1));
		auto voice = controls->getVoices()[j];
		ams1->setInputProcess(voice);
		auto mi = new MpcMixerInterconnection("con" + to_string(j), server);
		ams1->setDirectOutputProcess(mi->getInputProcess());
		auto ams2 = mixer->getStrip(to_string(j + 1 + 32));
		ams2->setInputProcess(mi->getOutputProcess());
		mixerConnections.push_back(mi);
	}
}

vector<shared_ptr<ctoot::mpc::MpcStereoMixerChannel>>& MpcSoundPlayerChannel::getStereoMixerChannels()
{
    return weakStereoMixerChannels;
}

vector<shared_ptr<ctoot::mpc::MpcIndivFxMixerChannel>>& MpcSoundPlayerChannel::getIndivFxMixerChannels()
{
	return weakIndivFxMixerChannels;
}

void MpcSoundPlayerChannel::mpcNoteOff(int note, int frameOffset, int noteOnStartTick)
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

void MpcSoundPlayerChannel::startDecayForNote(const int note, const int frameOffset, const int noteOnStartTick)
{
    for (auto& voice : controls->getVoices())
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

void MpcSoundPlayerChannel::stopMonoOrPolyVoiceWithSameNoteParameters(
        ctoot::mpc::MpcNoteParameters* noteParameters, int note)
{
    for (auto& voice : controls->getVoices())
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

MpcSoundPlayerChannel::~MpcSoundPlayerChannel() {
	for (auto& m : mixerConnections)
	{
		if (m != nullptr)
			delete m;
	}
}
