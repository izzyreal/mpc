#include "MixerScreen.hpp"

#include <lcdgui/screens/DrumScreen.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>

#include <sampler/Pad.hpp>

#include <sequencer/MixerEvent.hpp>
#include <sequencer/Track.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>

using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui;
using namespace moduru::lang;
using namespace std;

MixerScreen::MixerScreen(const int layerIndex) 
	: ScreenComponent("select-drum", layerIndex)
{
}

void MixerScreen::open()
{
	mpc.addObserver(this);
	
	initPadNameLabels();
	initMixerStrips();
	
	for (auto& m : mixerStrips)
	{
		m.lock()->initLabels();
		m.lock()->setColors();
	}
	
	displayMixerStrips();
	displayFunctionKeys();
	setXPos(mpc.getPad());
}

void MixerScreen::close()
{
	mpc.deleteObserver(this);
}

void MixerScreen::initPadNameLabels()
{
	int lCounter3 = 0;
	int lCounter4 = 0;

	for (auto label : findLabels())
	{
		auto l = label.lock();

		if (l)
		{
			if (l->getName().compare("e3") == 0 || l->getName().compare("e4") == 0)
			{
				l->setSize(6, 9);
			}

			if (l->getName().length() == 2)
			{
				if (l->getName()[1] == '3')
				{
					l->setText("0");

					if (lCounter3 > 8)
					{
						l->setText("1");
					}

					lCounter3++;
				}
				else if (l->getName()[1] == '4')
				{
					l->setText(to_string(lCounter4 + 1));
					lCounter4++;

					if (lCounter4 == 9)
					{
						lCounter4 = -1;
					}
				}
			}
		}
	}
}

void MixerScreen::initMixerStrips()
{
	deleteChildren("mixer-strip");

	mixerStrips.clear();

	init();

	for (int i = 0; i < 16; i++)
	{
		vector<weak_ptr<Label>> labels;

		for (int j = 0; j < 5; j++)
		{
			labels.push_back(findLabel(letters[i] + "0"));
		}

		mixerStrips.push_back(move(dynamic_pointer_cast<MixerStrip>(addChild(make_shared<MixerStrip>(i, mpc.getBank(), labels)).lock())));
	}
	
	displayMixerStrips();
	mixerStrips[xPos].lock()->setSelection(yPos);
}

void MixerScreen::displayMixerStrips()
{
	init();

	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));

	bool sSrcDrum = mixerSetupScreen->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	for (int i = 0; i < 16; i++)
	{
		auto smc = program.lock()->getPad(i + (mpc.getBank()* 16))->getStereoMixerChannel().lock();
		auto ifmc = program.lock()->getPad(i + (mpc.getBank()* 16))->getIndivFxMixerChannel().lock();

		if (sSrcDrum)
		{
			smc = mpcSoundPlayerChannel->getStereoMixerChannels()[i + (mpc.getBank() * 16)].lock();
		}

		if (iSrcDrum)
		{
			ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels()[i + (mpc.getBank() * 16)].lock();
		}

		if (tab == 0)
		{
			mixerStrips[i].lock()->setValueA(smc->getPanning());
			mixerStrips[i].lock()->setValueB(smc->getLevel());
		}
		else if (tab == 1)
		{
			if (smc->isStereo())
			{
				mixerStrips[i].lock()->setValueAString(stereoNames[ifmc->getOutput()]);
			}
			else
			{
				mixerStrips[i].lock()->setValueAString(monoNames[ifmc->getOutput()]);
			}
			mixerStrips[i].lock()->setValueB(ifmc->getVolumeIndividualOut());
		}
		else if (tab == 2)
		{
			mixerStrips[i].lock()->setValueAString(fxPathNames[ifmc->getFxPath()]);
			mixerStrips[i].lock()->setValueB(ifmc->getFxSendLevel());
		}
	}
}

void MixerScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	
	init();

	if (s.compare("bank") == 0)
	{
		initPadNameLabels();
		initMixerStrips();

		for (auto& m : mixerStrips)
		{
			m.lock()->initLabels();
			m.lock()->setColors();
		}
		
		displayMixerStrips();
		displayFunctionKeys();
	}
	else if (s.compare("padandnote") == 0)
	{
		setXPos(mpc.getPad());
	}
}

void MixerScreen::displayFunctionKeys()
{
	if (tab == 0)
	{
		ls.lock()->setFunctionKeysArrangement(link ? 3 : 0);
	}
	else if (tab == 1)
	{
		ls.lock()->setFunctionKeysArrangement(link ? 4 : 1);
	}
	else if (tab == 2)
	{
		ls.lock()->setFunctionKeysArrangement(link ? 5 : 2);
	}
}

void MixerScreen::setLink(bool b)
{
	link = b;

	if (tab == 0)
	{
		displayStereoFaders();
		displayPanning();
	}
	else if (tab == 1)
	{
		displayIndivFaders();
		displayIndividualOutputs();
	}
	else if (tab == 3)
	{
		displayFxSendLevels();
		displayFxPaths();
	}


	if (link)
	{
		for (auto& m : mixerStrips)
		{
			m.lock()->setSelection(yPos);
		}
	}
	else {
		for (auto& m : mixerStrips)
		{
			m.lock()->setSelection(-1);
		}
		mixerStrips[xPos].lock()->setSelection(yPos);
	}
	displayFunctionKeys();
}

void MixerScreen::setTab(int i)
{
	tab = i;

	initPadNameLabels();
	initMixerStrips();

	for (auto& m : mixerStrips)
	{
		m.lock()->initLabels();
		m.lock()->setColors();
	}
	displayMixerStrips();
	displayFunctionKeys();
}

int MixerScreen::getTab()
{
	return tab;
}

void MixerScreen::setXPos(int i)
{
	if (i < 0 || i > 15) return;
	xPos = i;
	if (!link) {
		for (auto& m : mixerStrips) {
			m.lock()->setSelection(-1);
		}
		mixerStrips[xPos].lock()->setSelection(yPos);
	}
	else {
		for (auto& m : mixerStrips) {
			m.lock()->setSelection(yPos);
		}
	}
}

void MixerScreen::setYPos(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}
	
	yPos = i;
	
	if (!link)
	{
		for (auto& m : mixerStrips)
		{
			m.lock()->setSelection(-1);
		}
		mixerStrips[xPos].lock()->setSelection(yPos);
	}
	else
	{
		for (auto& m : mixerStrips)
		{
			m.lock()->setSelection(yPos);
		}
	}
}

void MixerScreen::pad(int i, int velo, bool repeat, int tick) {
	setXPos(i);
	BaseControls::pad(i, velo, repeat, tick);
}

void MixerScreen::up()
{
	setYPos(yPos - 1);
}

void MixerScreen::down()
{
	setYPos(yPos + 1);
}

void MixerScreen::left()
{
	setXPos(xPos - 1);
}

void MixerScreen::right()
{
	setXPos(xPos + 1);
}

void MixerScreen::openWindow()
{
	init();
	
	int pad = xPos + (mpc.getBank() * 16);
	int note = program.lock()->getPad(pad)->getNote();
	mpc.setPadAndNote(pad, note);
	ls.lock()->openScreen("channel-settings");
}

void MixerScreen::function(int f)
{
	init();
	switch (f)
	{
	case 0:
	case 1:
	case 2:
		setTab(f);
		ls.lock()->getCurrentBackground()->SetDirty();
		break;
	case 3:
		ls.lock()->openScreen("mixer-setup");
		break;
	case 4:
		ls.lock()->openScreen("fx-edit");
	case 5:
		setLink(!link);
		break;
	}
}

void MixerScreen::turnWheel(int i)
{
	init();
	
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));

	int pad = xPos + (mpc.getBank() * 16);
	
	bool sSrcDrum = mixerSetupScreen->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	auto drumScreen = dynamic_pointer_cast<DrumScreen>(Screens::getScreenComponent("drum"));

	auto drum = mpc.getDrum(drumScreen->drum);

	auto smc = sSrcDrum ? drum->getStereoMixerChannels()[pad].lock() : program.lock()->getPad(pad)->getStereoMixerChannel().lock();
	auto smcs = sSrcDrum ? drum->getStereoMixerChannels() : vector<weak_ptr<ctoot::mpc::MpcStereoMixerChannel>>(16);
	auto ifmc = iSrcDrum ? drum->getIndivFxMixerChannels()[pad].lock() : program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();
	auto ifmcs = iSrcDrum ? drum->getIndivFxMixerChannels() : vector<weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel>>(16);

	for (int i = 0; i < 16; i++)
	{
		if (!sSrcDrum)
		{
			smcs[i] = program.lock()->getPad(i + (mpc.getBank() * 16))->getStereoMixerChannel().lock();
		}

		if (!iSrcDrum)
		{
			ifmcs[i] = program.lock()->getPad(i + (mpc.getBank() * 16))->getIndivFxMixerChannel().lock();
		}
	}

	if (tab == 0)
	{
		bool record = sequencer.lock()->isRecordingOrOverdubbing() && mixerSetupScreen->isRecordMixChangesEnabled();
		if (yPos == 0)
		{
			if (!link)
			{
				smc->setPanning(smc->getPanning() + i);
				
				if (record)
				{
					recordMixerEvent(pad, 1, smc->getPanning());
				}
			}
			else
			{
				int padCounter = 0;
				
				for (auto mcTemp : smcs)
				{
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setPanning(lMcTemp->getPanning() + i);
					
					if (record)
					{
						recordMixerEvent(padCounter++, 1, lMcTemp->getPanning());
					}
				}
			}

			displayPanning();

		}
		else if (yPos == 1)
		{
			if (!link)
			{
				smc->setLevel(smc->getLevel() + i);
				
				if (record)
				{
					recordMixerEvent(pad, 0, smc->getLevel());
				}
			}
			else
			{
				auto padCounter = 0;
				for (auto mcTemp : smcs)
				{
					auto lMcTemp = mcTemp.lock();
				
					lMcTemp->setLevel(lMcTemp->getLevel() + i);
					
					if (record)
					{
						recordMixerEvent(padCounter++, 0, lMcTemp->getLevel());
					}
				}
			}

			displayStereoFaders();
		}
	}
	else if (tab == 1)
	{
		if (yPos == 0)
		{
			if (!link)
			{
				ifmc->setOutput(ifmc->getOutput() + i);
				return;
			}
			else
			{
				for (auto mcTemp : ifmcs)
				{
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setOutput(lMcTemp->getOutput() + i);
				}
			}

			displayIndividualOutputs();

		}
		else if (yPos == 1)
		{
			if (!link)
			{
				ifmc->setVolumeIndividualOut(ifmc->getVolumeIndividualOut() + i);
			}
			else
			{
				for (auto mcTemp : ifmcs)
				{
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setVolumeIndividualOut(lMcTemp->getVolumeIndividualOut() + i);
				}
			}

			displayIndivFaders();
		}
	}
	else if (tab == 2)
	{
		if (yPos == 0)
		{
			if (!link)
			{
				ifmc->setFxPath(ifmc->getFxPath() + i);
			}
			else
			{
				for (auto mcTemp : ifmcs)
				{
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setFxPath(lMcTemp->getFxPath() + i);
				}
			}

			displayFxPaths();
		}
		else if (yPos == 1)
		{
			if (!link)
			{
				ifmc->setFxSendLevel(ifmc->getFxSendLevel() + i);
			}
			else
			{
				for (auto mcTemp : ifmcs)
				{
					auto lMcTemp = mcTemp.lock();
					lMcTemp->setFxSendLevel(lMcTemp->getFxSendLevel() + i);
				}
			}

			displayFxSendLevels();
		}
	}
}

void MixerScreen::recordMixerEvent(int pad, int param, int value)
{
	auto track = sequencer.lock()->getActiveSequence().lock()->getTrack(sequencer.lock()->getActiveTrackIndex()).lock();
	auto e = dynamic_pointer_cast<mpc::sequencer::MixerEvent>(track->addEvent(sequencer.lock()->getTickPosition(), "mixer").lock());
	e->setPadNumber(pad);
	e->setParameter(param);
	e->setValue(value);
}

void MixerScreen::displayStereoFaders()
{
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));

	bool sourceIsDrum = mixerSetupScreen->isStereoMixSourceDrum();

	if (link)
	{
		for (int i = 0; i < 16; i++)
		{
			int pad = i + (mpc.getBank() * 16);
			auto mc = program.lock()->getPad(pad)->getStereoMixerChannel().lock();
			if (sourceIsDrum)
			{
				mc = mpcSoundPlayerChannel->getStereoMixerChannels()[pad].lock();
			}
			mixerStrips[i].lock()->setValueB(mc->getLevel());
		}
	}
	else {
		int pad = xPos + (mpc.getBank() * 16);
		auto mixerChannel = program.lock()->getPad(pad)->getStereoMixerChannel();
		if (sourceIsDrum)
		{
			mixerChannel = mpcSoundPlayerChannel->getStereoMixerChannels()[pad].lock();
		}
		auto lMc = mixerChannel.lock();
		mixerStrips[xPos].lock()->setValueB(lMc->getLevel());
	}
}
void MixerScreen::displayPanning()
{
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));
	bool sourceIsDrum = mixerSetupScreen->isStereoMixSourceDrum();

	if (link)
	{
		for (int i = 0; i < 16; i++)
		{
			int pad = i + (mpc.getBank() * 16);
			auto mc = program.lock()->getPad(pad)->getStereoMixerChannel().lock();
			if (sourceIsDrum) mc = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
			mixerStrips[i].lock()->setValueA(mc->getPanning());
		}
	}
	else
	{
		int pad = xPos + (mpc.getBank() * 16);
		auto mixerChannel = program.lock()->getPad(pad)->getStereoMixerChannel();
		if (sourceIsDrum) mixerChannel = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
		auto lMc = mixerChannel.lock();
		mixerStrips[xPos].lock()->setValueA(lMc->getPanning());
	}
}

void MixerScreen::displayIndividualOutputs()
{

	init();
	
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));
	bool sSrcDrum = mixerSetupScreen->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	if (!link)
	{
		int pad = xPos + (mpc.getBank() * 16);
		auto smc = program.lock()->getPad(pad)->getStereoMixerChannel().lock();
		auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

		if (sSrcDrum)
		{
			smc = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
		}
		
		if (iSrcDrum)
		{
			ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
		}

		if (smc->isStereo())
		{
			mixerStrips[xPos].lock()->setValueAString(stereoNames[ifmc->getOutput()]);
		}
		else
		{
			mixerStrips[xPos].lock()->setValueAString(monoNames[ifmc->getOutput()]);
		}
	}
	else {
		for (int i = 0; i < 16; i++)
		{
			int pad = i + (mpc.getBank() * 16);
			auto smc = program.lock()->getPad(pad)->getStereoMixerChannel().lock();
			auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();
			
			if (sSrcDrum)
			{
				smc = mpcSoundPlayerChannel->getStereoMixerChannels().at(pad).lock();
			}
			
			if (iSrcDrum)
			{
				ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
			}
			
			if (smc->isStereo())
			{
				mixerStrips[i].lock()->setValueAString(stereoNames[ifmc->getOutput()]);
			}
			else
			{
				mixerStrips[i].lock()->setValueAString(monoNames[ifmc->getOutput()]);
			}
		}
	}
}

void MixerScreen::displayIndivFaders()
{
	init();
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	if (!link)
	{
		int pad = xPos + (mpc.getBank() * 16);
		auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

		if (iSrcDrum)
		{
			ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
		}
		mixerStrips[xPos].lock()->setValueB(ifmc->getVolumeIndividualOut());
	}
	else {
		for (int i = 0; i < 16; i++)
		{
			int pad = i + (mpc.getBank() * 16);
			auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();
			if (iSrcDrum)
			{
				ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
			}
			mixerStrips[i].lock()->setValueB(ifmc->getVolumeIndividualOut());
		}
	}
}

void MixerScreen::displayFxPaths()
{
	init();
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	if (!link)
	{
		int pad = xPos + (mpc.getBank() * 16);
		auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

		if (iSrcDrum)
		{
			ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
		}
		mixerStrips[xPos].lock()->setValueAString(fxPathNames[ifmc->getFxPath()]);
	}
	else {
		for (int i = 0; i < 16; i++)
		{
			int pad = i + (mpc.getBank() * 16);
			auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();
			if (iSrcDrum)
			{
				ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
			}
			mixerStrips[i].lock()->setValueAString(fxPathNames[ifmc->getFxPath()]);
		}
	}
}

void MixerScreen::displayFxSendLevels()
{
	init();
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(Screens::getScreenComponent("mixer-setup"));
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	if (!link)
	{
		int pad = xPos + (mpc.getBank() * 16);
		auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();
		if (iSrcDrum)
		{
			ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
		}
		mixerStrips[xPos].lock()->setValueB(ifmc->getFxSendLevel());
	}
	else {
		for (int i = 0; i < 16; i++)
		{
			int pad = i + (mpc.getBank() * 16);
			auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();
			if (iSrcDrum)
			{
				ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels().at(pad).lock();
			}
			mixerStrips[i].lock()->setValueB(ifmc->getFxSendLevel());
		}
	}
}

int MixerScreen::getXPos()
{
	return xPos;
}
