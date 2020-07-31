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

MixerScreen::MixerScreen(mpc::Mpc& mpc, const int layerIndex) 
	: ScreenComponent(mpc, "mixer", layerIndex)
{
	addMixerStrips();
}

void MixerScreen::open()
{
	mpc.addObserver(this);

	findField("dummy").lock()->Hide(true);
	
	for (auto& m : mixerStrips)
	{
		m.lock()->initLabels();
		m.lock()->setColors();
	}
	
	displayMixerStrips();
	displayFunctionKeys();
}

void MixerScreen::close()
{
	mpc.deleteObserver(this);
	findField("dummy").lock()->Hide(false);
}

void MixerScreen::addMixerStrips()
{
	for (int i = 0; i < 16; i++)
	{
		mixerStrips.push_back(move(dynamic_pointer_cast<MixerStrip>(addChild(make_shared<MixerStrip>(mpc, i)).lock())));
		mixerStrips.back().lock()->setBank(mpc.getBank());
	}
	
	displayMixerStrips();
	mixerStrips[getXPos()].lock()->setSelection(yPos);
}

void MixerScreen::displayMixerStrip(int i)
{
	init();

	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));

	bool sSrcDrum = mixerSetupScreen->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	auto smc = program.lock()->getPad(i + (mpc.getBank() * 16))->getStereoMixerChannel().lock();
	auto ifmc = program.lock()->getPad(i + (mpc.getBank() * 16))->getIndivFxMixerChannel().lock();

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

void MixerScreen::displayMixerStrips()
{
	for (int i = 0; i < 16; i++)
	{
		displayMixerStrip(i);
	}
}

void MixerScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);
	
	init();

	if (s.compare("bank") == 0)
	{
		for (auto& m : mixerStrips)
		{
			m.lock()->setBank(mpc.getBank());
		}
		displayMixerStrips();
	}
	else if (s.compare("padandnote") == 0)
	{
		if (link)
		{
			for (auto& m : mixerStrips)
			{
				m.lock()->setSelection(yPos);
			}
		}
		else
		{
			for (auto& m : mixerStrips)
			{
				m.lock()->setSelection(-1);
			}
			mixerStrips[getXPos()].lock()->setSelection(yPos);
		}
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
		mixerStrips[getXPos()].lock()->setSelection(yPos);
	}
	displayFunctionKeys();
}

void MixerScreen::setTab(int i)
{
	tab = i;

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


int MixerScreen::getXPos()
{
	return mpc.getPad() - (mpc.getBank() * 16);
}

void MixerScreen::setYPos(int i)
{
	if (i < 0 || i > 1)
	{
		return;
	}
	
	yPos = i;
	
	if (link)
	{
		for (auto& m : mixerStrips)
		{
			m.lock()->setSelection(yPos);
		}
	}
	else
	{
		for (auto& m : mixerStrips)
		{
			m.lock()->setSelection(-1);
		}
		mixerStrips[getXPos()].lock()->setSelection(yPos);
	}
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
	init();

	if (getXPos() <= 0)
	{
		return;
	}
	
	auto newPad = mpc.getPad() - 1;
	auto newNote = program.lock()->getNoteFromPad(newPad);
	mpc.setPadAndNote(newPad, newNote);
}

void MixerScreen::right()
{
	init();

	if (getXPos() >= 15)
	{
		return;
	}

	auto newPad = mpc.getPad() + 1;
	auto newNote = program.lock()->getNoteFromPad(newPad);
	mpc.setPadAndNote(newPad, newNote);
}

void MixerScreen::openWindow()
{
	init();
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
		break;
	case 3:
		ls.lock()->openScreen("mixer-setup");
		break;
//	case 4:
		//ls.lock()->openScreen("fx-edit");
	case 5:
		setLink(!link);
		break;
	}
}

void MixerScreen::turnWheel(int i)
{
	init();
	
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));

	int pad = getXPos() + (mpc.getBank() * 16);
	
	bool stereoMixSourceIsDrum = mixerSetupScreen->isStereoMixSourceDrum();
	bool indivAndFxSourceIsDrum = mixerSetupScreen->isIndivFxSourceDrum();

	auto drumScreen = dynamic_pointer_cast<DrumScreen>(mpc.screens->getScreenComponent("drum"));

	auto drum = mpc.getDrum(drumScreen->drum);

	auto stereoMixerChannels = stereoMixSourceIsDrum ? drum->getStereoMixerChannels() : vector<weak_ptr<ctoot::mpc::MpcStereoMixerChannel>>(16);
	auto indivFxMixerChannels = indivAndFxSourceIsDrum ? drum->getIndivFxMixerChannels() : vector<weak_ptr<ctoot::mpc::MpcIndivFxMixerChannel>>(16);

	for (int i = 0; i < 16; i++)
	{
		if (!stereoMixSourceIsDrum)
		{
			stereoMixerChannels[i] = program.lock()->getPad(i + (mpc.getBank() * 16))->getStereoMixerChannel().lock();
		}

		if (!indivAndFxSourceIsDrum)
		{
			indivFxMixerChannels[i] = program.lock()->getPad(i + (mpc.getBank() * 16))->getIndivFxMixerChannel().lock();
		}
	}

	if (tab == 0)
	{
		bool record = sequencer.lock()->isRecordingOrOverdubbing() && mixerSetupScreen->isRecordMixChangesEnabled();

		if (yPos == 0)
		{
			if (link)
			{
				int padCounter = 0;

				for (auto stereoMixerChannel : stereoMixerChannels)
				{
					auto mc = stereoMixerChannel.lock();
					mc->setPanning(mc->getPanning() + i);

					if (record)
					{
						recordMixerEvent(padCounter++, 1, mc->getPanning());
					}
				}
			}
			else
			{
				auto stereoMixerChannel = stereoMixSourceIsDrum ? drum->getStereoMixerChannels()[pad].lock() : program.lock()->getPad(pad)->getStereoMixerChannel().lock();
				stereoMixerChannel->setPanning(stereoMixerChannel->getPanning() + i);

				if (record)
				{
					recordMixerEvent(pad, 1, stereoMixerChannel->getPanning());
				}
			}
			displayPanning();
		}
		else if (yPos == 1)
		{
			if (link)
			{
				auto padCounter = 0;
				for (auto stereoMixerChannel : stereoMixerChannels)
				{
					auto mc = stereoMixerChannel.lock();

					mc->setLevel(mc->getLevel() + i);

					if (record)
					{
						recordMixerEvent(padCounter++, 0, mc->getLevel());
					}
				}
			}
			else
			{
				auto stereoMixerChannel = stereoMixSourceIsDrum ? drum->getStereoMixerChannels()[pad].lock() : program.lock()->getPad(pad)->getStereoMixerChannel().lock();
				stereoMixerChannel->setLevel(stereoMixerChannel->getLevel() + i);

				if (record)
				{
					recordMixerEvent(pad, 0, stereoMixerChannel->getLevel());
				}
			}

			displayStereoFaders();
		}
	}
	else if (tab == 1)
	{
		if (yPos == 0)
		{
			if (link)
			{
				for (auto mixerChannel : indivFxMixerChannels)
				{
					auto mc = mixerChannel.lock();
					mc->setOutput(mc->getOutput() + i);
				}
			}
			else
			{
				auto indivFxMixerChannel = indivAndFxSourceIsDrum ? drum->getIndivFxMixerChannels()[pad].lock() : program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();
				indivFxMixerChannel->setOutput(indivFxMixerChannel->getOutput() + i);
			}

			displayIndividualOutputs();
		}
		else if (yPos == 1)
		{
			if (link)
			{
				for (auto mixerChannel : indivFxMixerChannels)
				{
					auto mc = mixerChannel.lock();
					mc->setVolumeIndividualOut(mc->getVolumeIndividualOut() + i);
				}
			}
			else
			{
				auto indivFxMixerChannel = indivAndFxSourceIsDrum ? drum->getIndivFxMixerChannels()[pad].lock() : program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();
				indivFxMixerChannel->setVolumeIndividualOut(indivFxMixerChannel->getVolumeIndividualOut() + i);
			}

			displayIndivFaders();
		}
	}
	else if (tab == 2)
	{
		auto ifmc = indivAndFxSourceIsDrum ? drum->getIndivFxMixerChannels()[pad].lock() : program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

		if (yPos == 0)
		{
			if (link)
			{
				for (auto mixerChannel : indivFxMixerChannels)
				{
					auto mc = mixerChannel.lock();
					mc->setFxPath(mc->getFxPath() + i);
				}
			}
			else
			{
				ifmc->setFxPath(ifmc->getFxPath() + i);
			}

			displayFxPaths();
		}
		else if (yPos == 1)
		{
			if (link)
			{
				for (auto mixerChannel : indivFxMixerChannels)
				{
					auto mc = mixerChannel.lock();
					mc->setFxSendLevel(mc->getFxSendLevel() + i);
				}
			}
			else
			{
				ifmc->setFxSendLevel(ifmc->getFxSendLevel() + i);
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
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));

	bool sourceIsDrum = mixerSetupScreen->isStereoMixSourceDrum();

	if (link)
	{
		for (int i = 0; i < 16; i++)
		{
			auto mc = program.lock()->getPad(mpc.getPad())->getStereoMixerChannel().lock();
			if (sourceIsDrum)
			{
				mc = mpcSoundPlayerChannel->getStereoMixerChannels()[mpc.getPad()].lock();
			}
			mixerStrips[i].lock()->setValueB(mc->getLevel());
		}
	}
	else
	{
		auto mixerChannel = program.lock()->getPad(mpc.getPad())->getStereoMixerChannel();
	
		if (sourceIsDrum)
		{
			mixerChannel = mpcSoundPlayerChannel->getStereoMixerChannels()[mpc.getPad()].lock();
		}
		
		auto lMc = mixerChannel.lock();
		mixerStrips[getXPos()].lock()->setValueB(lMc->getLevel());
	}
}

void MixerScreen::displayPanning()
{
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));
	bool sourceIsDrum = mixerSetupScreen->isStereoMixSourceDrum();

	if (link)
	{
		for (int i = 0; i < 16; i++)
		{
			int pad = i + (mpc.getBank() * 16);
			auto mixerChannel = program.lock()->getPad(pad)->getStereoMixerChannel().lock();
			
			if (sourceIsDrum)
			{
				mixerChannel = mpcSoundPlayerChannel->getStereoMixerChannels()[pad].lock();
			}

			mixerStrips[i].lock()->setValueA(mixerChannel->getPanning());
		}
	}
	else
	{
		auto mixerChannel = program.lock()->getPad(mpc.getPad())->getStereoMixerChannel();
		
		if (sourceIsDrum)
		{
			mixerChannel = mpcSoundPlayerChannel->getStereoMixerChannels()[mpc.getPad()].lock();
		}

		mixerStrips[getXPos()].lock()->setValueA(mixerChannel.lock()->getPanning());
	}
}

void MixerScreen::displayIndividualOutputs()
{

	init();
	
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));
	bool sSrcDrum = mixerSetupScreen->isStereoMixSourceDrum();
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	if (link)
	{
		for (int i = 0; i < 16; i++)
		{
			auto pad = i + (mpc.getBank() * 16);
			auto stereoMixerChannel = program.lock()->getPad(pad)->getStereoMixerChannel().lock();
			auto indivFxMixerChannel = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

			if (sSrcDrum)
			{
				stereoMixerChannel = mpcSoundPlayerChannel->getStereoMixerChannels()[pad].lock();
			}

			if (iSrcDrum)
			{
				indivFxMixerChannel = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
			}

			if (stereoMixerChannel->isStereo())
			{
				mixerStrips[i].lock()->setValueAString(stereoNames[indivFxMixerChannel->getOutput()]);
			}
			else
			{
				mixerStrips[i].lock()->setValueAString(monoNames[indivFxMixerChannel->getOutput()]);
			}
		}
	}
	else {
		auto pad = mpc.getPad();

		auto smc = program.lock()->getPad(pad)->getStereoMixerChannel().lock();
		auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

		if (sSrcDrum)
		{
			smc = mpcSoundPlayerChannel->getStereoMixerChannels()[pad].lock();
		}

		if (iSrcDrum)
		{
			ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
		}

		if (smc->isStereo())
		{
			mixerStrips[getXPos()].lock()->setValueAString(stereoNames[ifmc->getOutput()]);
		}
		else
		{
			mixerStrips[getXPos()].lock()->setValueAString(monoNames[ifmc->getOutput()]);
		}
	}
}

void MixerScreen::displayIndivFaders()
{
	init();

	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	if (link)
	{
		for (int i = 0; i < 16; i++)
		{
			auto pad = i + (mpc.getBank() * 16);
			auto mixerChannel = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

			if (iSrcDrum)
			{
				mixerChannel = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
			}

			mixerStrips[i].lock()->setValueB(mixerChannel->getVolumeIndividualOut());
		}
	}
	else {
		auto pad = mpc.getPad();

		auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

		if (iSrcDrum)
		{
			ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
		}

		mixerStrips[getXPos()].lock()->setValueB(ifmc->getVolumeIndividualOut());
	}
}

void MixerScreen::displayFxPaths()
{
	init();
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	if (link)
	{
		for (int i = 0; i < 16; i++)
		{
			auto pad = i + (mpc.getBank() * 16);
			auto mixerChannel = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

			if (iSrcDrum)
			{
				mixerChannel = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
			}

			mixerStrips[i].lock()->setValueAString(fxPathNames[mixerChannel->getFxPath()]);
		}
	}
	else
	{
		auto pad = mpc.getPad();

		auto mixerChannel = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

		if (iSrcDrum)
		{
			mixerChannel = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
		}

		mixerStrips[getXPos()].lock()->setValueAString(fxPathNames[mixerChannel->getFxPath()]);
	}
}

void MixerScreen::displayFxSendLevels()
{
	init();
	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));
	bool iSrcDrum = mixerSetupScreen->isIndivFxSourceDrum();

	if (link)
	{
		for (int i = 0; i < 16; i++)
		{
			auto pad = i + (mpc.getBank() * 16);
			auto ifmc = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();
			if (iSrcDrum)
			{
				ifmc = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
			}
			mixerStrips[i].lock()->setValueB(ifmc->getFxSendLevel());
		}
	}
	else
	{
		auto pad = mpc.getPad();
		auto mixerChannel = program.lock()->getPad(pad)->getIndivFxMixerChannel().lock();

		if (iSrcDrum)
		{
			mixerChannel = mpcSoundPlayerChannel->getIndivFxMixerChannels()[pad].lock();
		}

		mixerStrips[getXPos()].lock()->setValueB(mixerChannel->getFxSendLevel());
	}
}
