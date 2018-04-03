#include "ChannelSettingsObserver.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <StartUp.hpp>
#include <lcdgui/Field.hpp>
#include <ui/sampler/MixerGui.hpp>
#include <ui/sampler/MixerSetupGui.hpp>
#include <ui/sampler/SamplerGui.hpp>
#include <sampler/NoteParameters.hpp>
#include <sampler/Pad.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

#include <lang/StrUtil.hpp>

#include <cmath>

using namespace mpc::ui::sampler;
using namespace std;

ChannelSettingsObserver::ChannelSettingsObserver(mpc::Mpc* mpc)
{
	this->mpc = mpc;
	fxPathNames = { "--", "M1", "M2", "R1", "R2" };
	stereoNamesSlash = { "-", "1/2", "1/2", "3/4", "3/4", "5/6", "5/6", "7/8", "7/8" };
	auto uis = mpc->getUis().lock();
	samplerGui = uis->getSamplerGui();
	samplerGui->addObserver(this);
	bank = samplerGui->getBank();
	mixGui = uis->getMixerGui();
	mixGui->addObserver(this);
	ls = mpc->getLayeredScreen();
	auto lLs = ls.lock();
	sampler = mpc->getSampler();
	mixerSetupGui = uis->getMixerSetupGui();
	mixerSetupGui->addObserver(this);
	auto lSampler = sampler.lock();
	mpcSoundPlayerChannel = lSampler->getDrum(samplerGui->getSelectedDrum());
	program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock());

	for (int i = (bank * 16); i < (bank * 16) + 16; i++) {
		auto pad = program.lock()->getPad(i);
		auto stMixerChannel = pad->getStereoMixerChannel().lock();
		auto indivFxMixerChannel = pad->getIndivFxMixerChannel().lock();
		stMixerChannel->addObserver(this);
		indivFxMixerChannel->addObserver(this);
	}

	noteField = lLs->lookupField("note");
	stereoVolumeField = lLs->lookupField("stereovolume");
	individualVolumeField = lLs->lookupField("individualvolume");
	fxSendLevelField = lLs->lookupField("fxsendlevel");
	panningField = lLs->lookupField("panning");
	outputField = lLs->lookupField("output");
	fxPathField = lLs->lookupField("fxpath");
	followStereoField = lLs->lookupField("followstereo");
	displayChannel();
}

void ChannelSettingsObserver::update(moduru::observer::Observable* o, boost::any arg)
{
	auto lLs = ls.lock();
	auto lProgram = program.lock();

	for (int i = (bank * 16); i < (bank * 16) + 16; i++) {
		auto pad = lProgram->getPad(i);
		auto stMixerChannel = pad->getStereoMixerChannel().lock();
		auto indivFxMixerChannel = pad->getIndivFxMixerChannel().lock();
		stMixerChannel->deleteObserver(this);
		indivFxMixerChannel->deleteObserver(this);
	}

	bank = samplerGui->getBank();

	for (int i = (bank * 16); i < (bank * 16) + 16; i++) {
		auto pad = lProgram->getPad(i);
		auto stMixerChannel = pad->getStereoMixerChannel().lock();
		auto indivFxMixerChannel = pad->getIndivFxMixerChannel().lock();
		stMixerChannel->addObserver(this);
		indivFxMixerChannel->addObserver(this);
	}


	string s = boost::any_cast<string>(arg);

	if (s.compare("padandnote") == 0) {
		displayChannel();
	}
	else if (s.compare("volume") == 0) {
		displayStereoVolume();
	}
	else if (s.compare("volumeindividual") == 0) {
		displayIndividualVolume();
	}
	else if (s.compare("fxsendlevel") == 0) {
		displayFxSendLevel();
	}
	else if (s.compare("panning") == 0) {
		displayPanning();
	}
	else if (s.compare("output") == 0) {
		displayOutput();
	}
	else if (s.compare("fxpath") == 0) {
		displayFxPath();
	}
	else if (s.compare("followstereo") == 0) {
		displayFollowStereo();
	}
	else if (s.compare("bank") == 0) {
		displayChannel();
	}
}

void ChannelSettingsObserver::displayChannel()
{
	displayNoteField();
	displayStereoVolume();
	displayIndividualVolume();
	displayFxSendLevel();
	displayPanning();
	displayOutput();
	displayFxPath();
	displayFollowStereo();
}

void ChannelSettingsObserver::displayNoteField()
{
	auto lSampler = sampler.lock();
	string soundString = "OFF";
	auto lProgram = program.lock();
	auto sampleNumber = lProgram->getNoteParameters(samplerGui->getNote())->getSndNumber();
	if (sampleNumber > 0 && sampleNumber < lSampler->getSoundCount()) {
		soundString = lSampler->getSoundName(sampleNumber);
		if (!lSampler->getSound(sampleNumber).lock()->isMono()) {
			soundString += moduru::lang::StrUtil::padLeft("(ST)", " ", 23 - soundString.length());
		}
	}
	noteField.lock()->setText(to_string(samplerGui->getNote()) + "/" + lSampler->getPadName(samplerGui->getPad()) + "-" + soundString);
}

void ChannelSettingsObserver::displayStereoVolume() {
	auto lProgram = program.lock();
	auto mixerChannel = lProgram->getPad(samplerGui->getPad())->getStereoMixerChannel();
	auto lMc = mixerChannel.lock();
	stereoVolumeField.lock()->setTextPadded(lMc->getLevel(), " ");
}

void ChannelSettingsObserver::displayIndividualVolume() {
	auto lProgram = program.lock();
	auto mixerChannel = lProgram->getPad(samplerGui->getPad())->getIndivFxMixerChannel();
	auto lMc = mixerChannel.lock();
	individualVolumeField.lock()->setTextPadded(lMc->getVolumeIndividualOut(), " ");
}

void ChannelSettingsObserver::displayFxSendLevel() {
	auto lProgram = program.lock();
	auto mixerChannel = lProgram->getPad(samplerGui->getPad())->getIndivFxMixerChannel();
	auto lMc = mixerChannel.lock();
	fxSendLevelField.lock()->setTextPadded(lMc->getFxSendLevel(), " ");
}

void ChannelSettingsObserver::displayPanning()
{
	auto lProgram = program.lock();
	auto mixerChannel = lProgram->getPad(samplerGui->getPad())->getStereoMixerChannel();
	auto lMc = mixerChannel.lock();
	if (lMc->getPanning() != 0) {
		auto panning = "L";
		if (lMc->getPanning() > 0) panning = "R";
		panningField.lock()->setText(panning + moduru::lang::StrUtil::padLeft(to_string(abs(lMc->getPanning())), " ", 2));
	}
	else
	{
		panningField.lock()->setText("MID");
	}
}

void ChannelSettingsObserver::displayOutput() {
	auto lProgram = program.lock();
	auto indivFxMixerChannel = lProgram->getPad(samplerGui->getPad())->getIndivFxMixerChannel().lock();
	auto stereoMixerChannel = lProgram->getPad(samplerGui->getPad())->getStereoMixerChannel().lock();

	if (stereoMixerChannel->isStereo()) {
		outputField.lock()->setText(stereoNamesSlash[indivFxMixerChannel->getOutput()]);
	}
	else {
		outputField.lock()->setText(" " + to_string(indivFxMixerChannel->getOutput()));
	}
}

void ChannelSettingsObserver::displayFxPath() {
	auto lProgram = program.lock();
	auto mixerChannel = lProgram->getPad(samplerGui->getPad())->getIndivFxMixerChannel();
	auto lMc = mixerChannel.lock();
	fxPathField.lock()->setText(fxPathNames[lMc->getFxPath()]);
}

void ChannelSettingsObserver::displayFollowStereo() {
	auto lProgram = program.lock();
	auto mixerChannel = lProgram->getPad(mixGui->getXPos() + (bank * 16))->getIndivFxMixerChannel();
	auto lMc = mixerChannel.lock();
	followStereoField.lock()->setText(lMc->isFollowingStereo() ? "YES" : "NO");
}

ChannelSettingsObserver::~ChannelSettingsObserver() {
	mixerSetupGui->deleteObserver(this);
	mixGui->deleteObserver(this);
	samplerGui->deleteObserver(this);

	for (int i = (bank * 16); i < (bank * 16) + 16; i++) {
		auto pad = program.lock()->getPad(i);
		auto stereoMixerChannel = pad->getStereoMixerChannel().lock();
		auto indivFxMixerChannel = pad->getIndivFxMixerChannel().lock();
		stereoMixerChannel->deleteObserver(this);
		indivFxMixerChannel->deleteObserver(this);
	}
}
