#include "MixerObserver.hpp"

#include <Mpc.hpp>
#include <Util.hpp>
#include <Paths.hpp>
#include <lcdgui/MixerStrip.hpp>
#include <lcdgui/Layer.hpp>
#include <lcdgui/Field.hpp>
#include <lcdgui/Label.hpp>
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

MixerObserver::MixerObserver()  
{
	
	fxPathNames = { "--", "M1", "M2", "R1", "R2" };
	stereoNames = { "-", "12", "12", "34", "34", "56", "56", "78", "78" };
	monoNames = { "-", "1", "2", "3", "4", "5", "6", "7", "8" };
	auto uis = Mpc::instance().getUis().lock();
	samplerGui = uis->getSamplerGui();
	samplerGui->addObserver(this);
	bank = samplerGui->getBank();
	mixGui = uis->getMixerGui();
	mixGui->setXPos(samplerGui->getPad());
	mixGui->addObserver(this);
	ls = Mpc::instance().getLayeredScreen();
	auto lLs = ls.lock();
	sampler = Mpc::instance().getSampler();
	mixerSetupGui = uis->getMixerSetupGui();
	mixerSetupGui->addObserver(this);
	auto lSampler = sampler.lock();
	mpcSoundPlayerChannel = lSampler->getDrum(samplerGui->getSelectedDrum());
	program = dynamic_pointer_cast<mpc::sampler::Program>(lSampler->getProgram(mpcSoundPlayerChannel->getProgram()).lock());

	bool sSrcDrum = mixerSetupGui->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupGui->isIndivFxSourceDrum();

	for (int i = (bank * 16); i < (bank * 16) + 16; i++) {
		auto pad = program.lock()->getPad(i);
		auto stMixerChannel = sSrcDrum ? mpcSoundPlayerChannel->getStereoMixerChannels().at(i).lock() : pad->getStereoMixerChannel().lock();
		auto indivFxMixerChannel = iSrcDrum ? mpcSoundPlayerChannel->getIndivFxMixerChannels().at(i).lock() : pad->getIndivFxMixerChannel().lock();
		stMixerChannel->addObserver(this);
		indivFxMixerChannel->addObserver(this);
	}

	if (lLs->getCurrentScreenName().compare("mixer") == 0) {
		initPadNameLabels();
		initMixerStrips();
		for (auto& m : mixerStrips) {
			m->initLabels();
			m->setColors();
		}
		displayMixerStrips();
		displayFunctionKeys();
	}
	else if (lLs->getCurrentScreenName().compare("mixersetup") == 0) {
		masterLevelField = lLs->lookupField("masterlevel");
		fxDrumField = lLs->lookupField("fxdrum");
		stereoMixSourceField = lLs->lookupField("stereomixsource");
		indivFxSourceField = lLs->lookupField("indivfxsource");
		copyPgmMixToDrumField = lLs->lookupField("copypgmmixtodrum");
		recordMixChangesField = lLs->lookupField("recordmixchanges");
		displayMasterLevel();
		displayFxDrum();
		displayStereoMixSource();
		displayIndivFxSource();
		displayCopyPgmMixToDrum();
		displayRecordMixChanges();
	}
}

void MixerObserver::initPadNameLabels()
{
	int lCounter3 = 0;
	int lCounter4 = 0;
	for (auto label : ls.lock()->getLayer(0)->findLabels()) {
		auto l = dynamic_pointer_cast<mpc::lcdgui::Label>(label.lock());
		if (l) {
			if (l->getName().compare("e3") == 0 || l->getName().compare("e4") == 0) {
				l->setSize(6, 9);
			}
			if (l->getName().length() == 2) {
				if (l->getName()[1] == '3') {
					l->setText("0");
					if (lCounter3 > 8) {
						l->setText("1");
					}
					lCounter3++;
				}
				else if (l->getName()[1] == '4') {
					l->setText(to_string(lCounter4 + 1));
					lCounter4++;
					if (lCounter4 == 9) {
						lCounter4 = -1;
					}
				}
			}
		}
	}
}

void MixerObserver::initMixerStrips()
{
	mixerStrips.clear();
	for (int i = 0; i < 16; i++) {
		auto mixerStrip = new mpc::lcdgui::MixerStrip(i, bank);
		mixerStrips.push_back(mixerStrip);
	}
	mixGui->setMixerStrips(mixerStrips);
	displayMixerStrips();
	mixerStrips[mixGui->getXPos()]->setSelection(mixGui->getYPos());
}

void MixerObserver::displayMixerStrips()
{
	bool sSrcDrum = mixerSetupGui->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupGui->isIndivFxSourceDrum();

	for (int i = 0; i < 16; i++) {
		auto smc = program.lock()->getPad(i + (bank * 16))->getStereoMixerChannel().lock();
		auto ifmc = program.lock()->getPad(i + (bank * 16))->getIndivFxMixerChannel().lock();

		if (sSrcDrum) smc = mpcSoundPlayerChannel->getStereoMixerChannels().at(i + (bank * 16)).lock();
		if (iSrcDrum) ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(i + (bank * 16)).lock();

		if (mixGui->getTab() == 0) {
			mixerStrips[i]->setValueA(smc->getPanning());
			mixerStrips[i]->setValueB(smc->getLevel());
		}
		else if (mixGui->getTab() == 1) {
			if (smc->isStereo()) {
				mixerStrips[i]->setValueAString(stereoNames[ifmc->getOutput()]);
			}
			else {
				mixerStrips[i]->setValueAString(monoNames[ifmc->getOutput()]);
			}
			mixerStrips[i]->setValueB(ifmc->getVolumeIndividualOut());
		}
		else if (mixGui->getTab() == 2) {
			mixerStrips[i]->setValueAString(fxPathNames[ifmc->getFxPath()]);
			mixerStrips[i]->setValueB(ifmc->getFxSendLevel());
		}
	}
}

void MixerObserver::update(moduru::observer::Observable* o, nonstd::any arg)
{
	auto lLs = ls.lock();
	auto lProgram = program.lock();

	bool sSrcDrum = mixerSetupGui->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupGui->isIndivFxSourceDrum();

	for (int i = (bank * 16); i < (bank * 16) + 16; i++) {
		auto pad = lProgram->getPad(i);
		auto stMixerChannel = sSrcDrum ? mpcSoundPlayerChannel->getStereoMixerChannels().at(i).lock() : pad->getStereoMixerChannel().lock();
		auto indivFxMixerChannel = iSrcDrum ? mpcSoundPlayerChannel->getIndivFxMixerChannels().at(i).lock() : pad->getIndivFxMixerChannel().lock();
		stMixerChannel->deleteObserver(this);
		indivFxMixerChannel->deleteObserver(this);
	}

	bank = samplerGui->getBank();

	for (int i = (bank * 16); i < (bank * 16) + 16; i++) {
		auto pad = lProgram->getPad(i);
		auto stMixerChannel = sSrcDrum ? mpcSoundPlayerChannel->getStereoMixerChannels().at(i).lock() : pad->getStereoMixerChannel().lock();
		auto indivFxMixerChannel = iSrcDrum ? mpcSoundPlayerChannel->getIndivFxMixerChannels().at(i).lock() : pad->getIndivFxMixerChannel().lock();
		stMixerChannel->addObserver(this);
		indivFxMixerChannel->addObserver(this);
	}

	string s = nonstd::any_cast<string>(arg);

	if (s.compare("tab") == 0) {
		initPadNameLabels();
		initMixerStrips();
		for (auto& m : mixerStrips) {
			m->initLabels();
			m->setColors();
		}
		displayMixerStrips();
		displayFunctionKeys();
	}
	else if (s.compare("position") == 0) {
		if (!mixGui->getLink()) {
			for (auto& m : mixerStrips) {
				m->setSelection(-1);
			}
			mixerStrips[mixGui->getXPos()]->setSelection(mixGui->getYPos());
		}
		else {
			for (auto& m : mixerStrips) {
				m->setSelection(mixGui->getYPos());
			}
		}
	}
	else if (s.compare("volume") == 0) {
		if (lLs->getCurrentScreenName().compare("mixer") == 0) {
			if (!mixGui->getLink()) {
				int pad = mixGui->getXPos() + (bank * 16);
				auto mixerChannel = lProgram->getPad(pad)->getStereoMixerChannel();
				if (sSrcDrum) mixerChannel = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
				auto lMc = mixerChannel.lock();
				mixerStrips[mixGui->getXPos()]->setValueB(lMc->getLevel());
			}
			else {
				for (int i = 0; i < 16; i++) {
					int pad = i + (bank * 16);
					auto mc = lProgram->getPad(pad)->getStereoMixerChannel().lock();
					if (sSrcDrum) mc = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
					mixerStrips[i]->setValueB(mc->getLevel());
				}
			}
		}
	}
	else if (s.compare("panning") == 0) {
		if (lLs->getCurrentScreenName().compare("mixer") == 0) {
			if (!mixGui->getLink()) {
				int pad = mixGui->getXPos() + (bank * 16);
				auto mixerChannel = lProgram->getPad(pad)->getStereoMixerChannel();
				if (sSrcDrum) mixerChannel = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
				auto lMc = mixerChannel.lock();
				mixerStrips[mixGui->getXPos()]->setValueA(lMc->getPanning());
			}
			else {
				for (int i = 0; i < 16; i++) {
					int pad = i + (bank * 16);
					auto mc = lProgram->getPad(pad)->getStereoMixerChannel().lock();
					if (sSrcDrum) mc = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
					mixerStrips[i]->setValueA(mc->getPanning());
				}
			}
		}
	}
	else if (s.compare("output") == 0) {
		if (lLs->getCurrentScreenName().compare("mixer") == 0) {
			if (!mixGui->getLink()) {
				int pad = mixGui->getXPos() + (bank * 16);
				auto smc = lProgram->getPad(pad)->getStereoMixerChannel().lock();
				auto ifmc = lProgram->getPad(pad)->getIndivFxMixerChannel().lock();
				if (sSrcDrum) smc = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
				if (iSrcDrum) ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();

				if (smc->isStereo()) {
					mixerStrips[mixGui->getXPos()]->setValueAString(stereoNames[ifmc->getOutput()]);
				}
				else {
					mixerStrips[mixGui->getXPos()]->setValueAString(monoNames[ifmc->getOutput()]);
				}
			}
			else {
				for (int i = 0; i < 16; i++) {
					int pad = i + (bank * 16);
					auto smc = lProgram->getPad(pad)->getStereoMixerChannel().lock();
					auto ifmc = lProgram->getPad(pad)->getIndivFxMixerChannel().lock();
					if (sSrcDrum) smc = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
					if (iSrcDrum) ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
					if (smc->isStereo()) {
						mixerStrips[i]->setValueAString(stereoNames[ifmc->getOutput()]);
					}
					else {
						mixerStrips[i]->setValueAString(monoNames[ifmc->getOutput()]);
					}
				}
			}
		}
	}
	else if (s.compare("volumeindividual") == 0) {
		if (lLs->getCurrentScreenName().compare("mixer") == 0) {
			if (!mixGui->getLink()) {
				int pad = mixGui->getXPos() + (bank * 16);
				auto ifmc = lProgram->getPad(pad)->getIndivFxMixerChannel().lock();
				if (iSrcDrum) ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
				mixerStrips[mixGui->getXPos()]->setValueB(ifmc->getVolumeIndividualOut());
			}
			else {
				for (int i = 0; i < 16; i++) {
					int pad = i + (bank * 16);
					auto ifmc = lProgram->getPad(pad)->getIndivFxMixerChannel().lock();
					if (iSrcDrum) ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
					mixerStrips[i]->setValueB(ifmc->getVolumeIndividualOut());
				}
			}
		}
	}
	else if (s.compare("fxpath") == 0) {
		if (lLs->getCurrentScreenName().compare("mixer") == 0) {
			if (!mixGui->getLink()) {
				int pad = mixGui->getXPos() + (bank * 16);
				auto ifmc = lProgram->getPad(pad)->getIndivFxMixerChannel().lock();
				if (iSrcDrum) ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
				mixerStrips[mixGui->getXPos()]->setValueAString(fxPathNames[ifmc->getFxPath()]);
			}
			else {
				for (int i = 0; i < 16; i++) {
					int pad = i + (bank * 16);
					auto ifmc = lProgram->getPad(pad)->getIndivFxMixerChannel().lock();
					if (iSrcDrum) ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
					mixerStrips[i]->setValueAString(fxPathNames[ifmc->getFxPath()]);
				}
			}
		}
	}
	else if (s.compare("fxsendlevel") == 0) {
		if (lLs->getCurrentScreenName().compare("mixer") == 0) {
			if (!mixGui->getLink()) {
				int pad = mixGui->getXPos() + (bank * 16);
				auto ifmc = lProgram->getPad(pad)->getIndivFxMixerChannel().lock();
				if (iSrcDrum) ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
				mixerStrips[mixGui->getXPos()]->setValueB(ifmc->getFxSendLevel());
			}
			else {
				for (int i = 0; i < 16; i++) {
					int pad = i + (bank * 16);
					auto ifmc = lProgram->getPad(pad)->getIndivFxMixerChannel().lock();
					if (iSrcDrum) ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
					mixerStrips[i]->setValueB(ifmc->getFxSendLevel());
				}
			}
		}
	}
	else if (s.compare("link") == 0) {
		if (mixGui->getLink()) {
			for (auto& m : mixerStrips) {
				m->setSelection(mixGui->getYPos());
			}
		}
		else {
			for (auto& m : mixerStrips) {
				m->setSelection(-1);
			}
			mixerStrips[mixGui->getXPos()]->setSelection(mixGui->getYPos());
		}
		displayFunctionKeys();
	}
	else if (s.compare("bank") == 0) {
		ls.lock()->getCurrentBackground()->SetDirty();
		initPadNameLabels();
		initMixerStrips();
		for (auto& m : mixerStrips) {
			m->initLabels();
			m->setColors();
		}
		displayMixerStrips();
		displayFunctionKeys();
	}
	else if (s.compare("masterlevel") == 0) {
		displayMasterLevel();
	}
	else if (s.compare("fxdrum") == 0) {
		displayFxDrum();
	}
	else if (s.compare("stereomixsource") == 0) {
		displayStereoMixSource();
	}
	else if (s.compare("indivfxsource") == 0) {
		displayIndivFxSource();
	}
	else if (s.compare("copypgmmixtodrum") == 0) {
		displayCopyPgmMixToDrum();
	}
	else if (s.compare("recordmixchanges") == 0) {
		displayRecordMixChanges();
	}
}

void MixerObserver::displayFunctionKeys()
{
	string link = "";
	if (mixGui->getLink()) {
		link = "_link";
	}

	auto lLs = ls.lock();

	if (mixGui->getTab() == 0) {
		lLs->setFunctionKeysArrangement(mixGui->getLink() ? 3 : 0);
	}
	else if (mixGui->getTab() == 1) {
		lLs->setFunctionKeysArrangement(mixGui->getLink() ? 4 : 1);
	}
	else if (mixGui->getTab() == 2) {
		lLs->setFunctionKeysArrangement(mixGui->getLink() ? 5 : 2);
	}
}

void MixerObserver::displayMasterLevel() {
	auto level = mixerSetupGui->getMasterLevelString();
	if (mixerSetupGui->getMasterLevel() != -13) level = moduru::lang::StrUtil::padLeft(level, " ", 5);
	masterLevelField.lock()->setText(level);
}

void MixerObserver::displayFxDrum() {
	fxDrumField.lock()->setText(to_string(mixerSetupGui->getFxDrum() + 1));
}

void MixerObserver::displayStereoMixSource() {
	stereoMixSourceField.lock()->setText(mixerSetupGui->isStereoMixSourceDrum() ? "DRUM" : "PROGRAM");
}

void MixerObserver::displayIndivFxSource() {
	indivFxSourceField.lock()->setText(mixerSetupGui->isIndivFxSourceDrum() ? "DRUM" : "PROGRAM");
}

void MixerObserver::displayCopyPgmMixToDrum() {
	copyPgmMixToDrumField.lock()->setText(mixerSetupGui->isCopyPgmMixToDrumEnabled() ? "YES" : "NO");
}

void MixerObserver::displayRecordMixChanges() {
	recordMixChangesField.lock()->setText(mixerSetupGui->isRecordMixChangesEnabled() ? "YES" : "NO");
}


MixerObserver::~MixerObserver() {
	mixerSetupGui->deleteObserver(this);
	mixGui->deleteObserver(this);
	samplerGui->deleteObserver(this);

	bool sSrcDrum = mixerSetupGui->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupGui->isIndivFxSourceDrum();

	for (int i = (bank * 16); i < (bank * 16) + 16; i++) {
		auto pad = program.lock()->getPad(i);
		auto stMixerChannel = sSrcDrum ? mpcSoundPlayerChannel->getStereoMixerChannels().at(i).lock() : pad->getStereoMixerChannel().lock();
		auto indivFxMixerChannel = iSrcDrum ? mpcSoundPlayerChannel->getIndivFxMixerChannels().at(i).lock() : pad->getIndivFxMixerChannel().lock();
		stMixerChannel->deleteObserver(this);
		indivFxMixerChannel->deleteObserver(this);
	}

	for (auto& m : mixerStrips) {
		if (m != nullptr) delete m;
	}
}
