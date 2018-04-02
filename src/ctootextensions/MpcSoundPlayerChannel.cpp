#include "MpcSoundPlayerChannel.hpp"

// ctootextensions
#include "MpcSoundPlayerControls.hpp"
#include "MpcMultiMidiSynth.hpp"
#include "MpcFaderControl.hpp"
#include "MpcMixerInterconnection.hpp"
#include "Voice.hpp"
#include "MuteInfo.hpp"

// mpc
#include <sampler/NoteParameters.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>
#include <sampler/StereoMixerChannel.hpp>
#include <sampler/IndivFxMixerChannel.hpp>
#include <ui/sampler/MixerSetupGui.hpp>

// ctoot
#include <audio/core/MetaInfo.hpp>
#include <audio/core/AudioControlsChain.hpp>
#include <audio/mixer/AudioMixer.hpp>
#include <audio/mixer/AudioMixerStrip.hpp>
#include <audio/mixer/MainMixControls.hpp>
#include <audio/mixer/PanControl.hpp>
#include <audio/mixer/MixerControls.hpp>
#include <audio/server/AudioServer.hpp>
#include <control/CompoundControl.hpp>
#include <control/Control.hpp>

#include <Mpc.hpp>

using namespace mpc::ctootextensions;
using namespace std;

MpcSoundPlayerChannel::MpcSoundPlayerChannel(weak_ptr<MpcSoundPlayerControls> controls)
{
	this->controls = controls;
	receivePgmChange = true;
	receiveMidiVolume = true;
	auto lControls = controls.lock();
	index = lControls->getDrumNumber();
	sampler = lControls->getSampler();
	mixer = lControls->getMixer();
	server = lControls->getServer();
	for (int i = 0; i < 64; i++) {
		stereoMixerChannels.push_back(make_shared<mpc::sampler::StereoMixerChannel>());
		indivFxMixerChannels.push_back(make_shared<mpc::sampler::IndivFxMixerChannel>());
	}
}

void MpcSoundPlayerChannel::setProgram(int i)
{
	if (i < 0) return;
	if (!sampler.lock()->getProgram(i).lock()) return;
	programNumber = i;
	setChanged();
	notifyObservers(string("pgm"));
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
	setChanged();
	notifyObservers(string("receivepgmchange"));
}

bool MpcSoundPlayerChannel::receivesMidiVolume()
{
	return receiveMidiVolume;
}

void MpcSoundPlayerChannel::setReceiveMidiVolume(bool b)
{
	receiveMidiVolume = b;
	setChanged();
	notifyObservers(string("receivemidivolume"));
}

void MpcSoundPlayerChannel::setLocation(string location)
{
}

void MpcSoundPlayerChannel::noteOn(int note, int velo)
{
	mpcNoteOn(-1, note, velo, 0, 64, 0, true);
}

void MpcSoundPlayerChannel::mpcNoteOn(int track, int note, int velo, int varType, int varValue, int frameOffset, bool firstGeneration)
{
	if (note < 35 || note > 98) return;
	if (velo == 0) return;
	auto lSampler = sampler.lock();
	auto program = lSampler->getProgram(programNumber);
	auto lProgram = program.lock();
	auto padNumber = lProgram->getPadNumberFromNote(note);
	auto np = lProgram->getNoteParameters(note);
	checkForMutes(np);
	auto soundNumber = np->getSndNumber();

	weak_ptr<Voice> voice;
	auto lControls = controls.lock();
	for (auto& v : lControls->getMms().lock()->getVoices()) {
		if (v.lock()->isFinished()) {
			voice = v;
			break;
		}
	}

	if (soundNumber == -1 || !voice.lock()) {
		return;
	}

	auto vars = lSampler->getSound(soundNumber);
	auto smc = lProgram->getStereoMixerChannel(padNumber).lock();
	auto ifmc = lProgram->getIndivFxMixerChannel(padNumber).lock();

	bool sSrcDrum = controls.lock()->getMixerSetupGui()->isStereoMixSourceDrum();
	bool iSrcDrum = controls.lock()->getMixerSetupGui()->isIndivFxSourceDrum();
	if (sSrcDrum) smc = stereoMixerChannels[padNumber];
	if (iSrcDrum) ifmc = indivFxMixerChannels[padNumber];

	shared_ptr<ctoot::audio::mixer::AudioMixer> lMixer = mixer.lock();
	auto sc = lMixer->getMixerControls().lock()->getStripControls(to_string(voice.lock()->getStripNumber())).lock();
	
	// We set the FX send level.
	dynamic_pointer_cast<ctootextensions::MpcFaderControl>(dynamic_pointer_cast<CompoundControl>(sc->find("FX#1").lock())->find("Level").lock())->setValue(ifmc->getFxSendLevel());
	
	auto mmc = dynamic_pointer_cast<ctoot::audio::mixer::MainMixControls>(sc->find("Main").lock());
	dynamic_pointer_cast<ctoot::audio::mixer::PanControl>(mmc->find("Pan").lock())->setValue((smc->getPanning()) / 100.0);
	dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level").lock())->setValue(smc->getLevel());

	sc = lMixer->getMixerControls().lock()->getStripControls(to_string(voice.lock()->getStripNumber() + 32)).lock();
	mmc = dynamic_pointer_cast<ctoot::audio::mixer::MainMixControls>(sc->find("Main").lock());

	//We make sure the voice strip duplicages that are used for mixing to ASSIGNALE MIX OUT are not mixed into Main.
	auto faderControl = dynamic_pointer_cast<ctoot::audio::fader::FaderControl>(mmc->find("Level").lock());
	if (faderControl->getValue() != 0) faderControl->setValue(0);

	if (ifmc->getOutput() > 0) {
		if (vars.lock()->isMono()) {
			if (ifmc->getOutput() % 2 == 1) {
				mixerConnections[voice.lock()->getStripNumber() - 1]->setLeftEnabled(true);
				mixerConnections[voice.lock()->getStripNumber() - 1]->setRightEnabled(false);
			}
			else {
				mixerConnections[voice.lock()->getStripNumber() - 1]->setLeftEnabled(false);
				mixerConnections[voice.lock()->getStripNumber() - 1]->setRightEnabled(true);
			}
		}
		else {
			mixerConnections[voice.lock()->getStripNumber() - 1]->setLeftEnabled(true);
			mixerConnections[voice.lock()->getStripNumber() - 1]->setRightEnabled(true);
		}
	}

	/*
	* We iterate through AUX#1 - #4 (aka ASSIGNABLE MIX OUT 1/2, 3/4, 5/6 and 7/8).
	* We silence the aux buses that are not in use by this voice.
	* We set the level for the MIX OUT that is in use, if any.
	*/
	int selectedAssignableMixOutPair = (int)(ceil((ifmc->getOutput() - 2) / 2.0));
	for (int i = 0; i < 4; i++) {
		auto auxControl = dynamic_pointer_cast<ctoot::control::CompoundControl>(sc->find("AUX#" + to_string(i + 1)).lock());
		auto auxLevel = dynamic_pointer_cast<MpcFaderControl>(auxControl->find("Level").lock());
		if (i == selectedAssignableMixOutPair) {
			auxLevel->setValue(ifmc->getVolumeIndividualOut());
		}
		else {
			auxLevel->setValue(0);
		}
	}
	stopPad(padNumber, 1);
	voice.lock()->init(track, velo, padNumber, vars, np, varType, varValue, note, index, frameOffset, true);

	if (firstGeneration) {
		if (np->getSoundGenerationMode() == 1) {
			int optA = np->getOptionalNoteA();
			int optB = np->getOptionalNoteB();
			if (optA != 34) {
				mpcNoteOn(track, optA, velo, varType, varValue, frameOffset, false);
				simultA[note] = optA;
			}
			if (optB != 34) {
				mpcNoteOn(track, optB, velo, varType, varValue, frameOffset, false);
				simultB[note] = optB;
			}
		}
	}
}

void MpcSoundPlayerChannel::checkForMutes(mpc::sampler::NoteParameters* np)
{
	if (np->getMuteAssignA() != 34 || np->getMuteAssignB() != 34) {
		for (auto& v : controls.lock()->getMms().lock()->getVoices()) {
			if (v.lock()->isFinished()) continue;
			if (v.lock()->getMuteInfo() == nullptr) {
				continue;
			}

			if (v.lock()->getMuteInfo()->muteMe(np->getMuteAssignA(), index)
				|| v.lock()->getMuteInfo()->muteMe(np->getMuteAssignB(), index)) {
				v.lock()->startDecay();
			}
		}
	}
}

void MpcSoundPlayerChannel::stopPad(int p, int o)
{
	for (auto& v : controls.lock()->getMms().lock()->getVoices()) {
		if (v.lock()->getPadNumber() == p
			&& v.lock()->getVoiceOverlap() == o
			&& !v.lock()->isDecaying()
			&& index == v.lock()->getMuteInfo()->getDrum()) {
			v.lock()->startDecay();
			break;
		}
	}
}

void MpcSoundPlayerChannel::noteOff(int note)
{
	mpcNoteOff(note, 0);
}

void MpcSoundPlayerChannel::allNotesOff()
{
}

void MpcSoundPlayerChannel::allSoundOff()
{
	for (auto& voice : controls.lock()->getMms().lock()->getVoices()) {
		if (voice.lock()->isFinished()) continue;
		voice.lock()->startDecay();
	}
}

void MpcSoundPlayerChannel::allSoundOff(int frameOffset)
{
	for (auto& voice : controls.lock()->getMms().lock()->getVoices()) {
		if (voice.lock()->isFinished()) continue;
		voice.lock()->startDecay(frameOffset);
	}
}

void MpcSoundPlayerChannel::connectVoices()
{
	auto lMixer = mixer.lock();
	for (auto j = 0; j < 32; j++) {
		auto ams1 = lMixer->getStrip(to_string(j + 1)).lock();
		auto voice = controls.lock()->getMms().lock()->getVoices()[j];
		ams1->setInputProcess(voice);
		auto mi = new MpcMixerInterconnection("con" + to_string(j), server);
		ams1->setDirectOutputProcess(mi->getInputProcess());
		auto ams2 = lMixer->getStrip(to_string(j + 1 + 32)).lock();
		ams2->setInputProcess(mi->getOutputProcess());
		mixerConnections.push_back(mi);
	}
}

weak_ptr<ctoot::audio::core::MetaInfo> MpcSoundPlayerChannel::getInfo()
{
	return weak_ptr<ctoot::audio::core::MetaInfo>();
}

vector<weak_ptr<mpc::sampler::StereoMixerChannel>> MpcSoundPlayerChannel::getStereoMixerChannels()
{
	vector<weak_ptr<mpc::sampler::StereoMixerChannel>> res;
	for (auto& m : stereoMixerChannels)
		res.push_back(m);
	return res;
}

vector<weak_ptr<mpc::sampler::IndivFxMixerChannel>> MpcSoundPlayerChannel::getIndivFxMixerChannels()
{
	vector<weak_ptr<mpc::sampler::IndivFxMixerChannel>> res;
	for (auto& m : indivFxMixerChannels)
		res.push_back(m);
	return res;
}

int MpcSoundPlayerChannel::getDrumNumber()
{
	return index;
}

void MpcSoundPlayerChannel::mpcNoteOff(int note, int frameOffset)
{
	if (note < 35 || note > 98) {
		return;
	}
	stopPad(sampler.lock()->getProgram(programNumber).lock()->getPadNumberFromNote(note), 2, frameOffset);
	std::map<int, int>::iterator it = simultA.find(note);
	if (it != simultA.end()) {
		stopPad(sampler.lock()->getProgram(programNumber).lock()->getPadNumberFromNote(simultA[note]), 2);
		simultA.erase(it);
	}
	it = simultB.find(note);
	if (it != simultB.end()) {
		stopPad(sampler.lock()->getProgram(programNumber).lock()->getPadNumberFromNote(simultB[note]), 2);
		simultB.erase(it);
	}
}

void MpcSoundPlayerChannel::stopPad(int pad, int overlap, int offset)
{
	for (auto& v : controls.lock()->getMms().lock()->getVoices()) {
		if (v.lock()->getPadNumber() == pad
			&& v.lock()->getVoiceOverlap() == overlap
			&& !v.lock()->isDecaying()
			&& index == v.lock()->getMuteInfo()->getDrum()) {
			v.lock()->startDecay(offset);
			break;
		}
	}
}

MpcSoundPlayerChannel::~MpcSoundPlayerChannel() {
	for (auto& m : mixerConnections) {
		if (m != nullptr) {
			delete m;
		}
	}
}
