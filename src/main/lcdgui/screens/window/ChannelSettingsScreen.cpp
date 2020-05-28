#include "ChannelSettingsScreen.hpp"

#include <sampler/Pad.hpp>

#include <lcdgui/screens/MixerScreen.hpp>

#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace moduru::lang;
using namespace std;

ChannelSettingsScreen::ChannelSettingsScreen(const int layerIndex)
	: ScreenComponent("channel-settings", layerIndex)
{
}

void ChannelSettingsScreen::open()
{
	mpc.addObserver(this);
}

void ChannelSettingsScreen::close()
{
	mpc.deleteObserver(this);
}

void ChannelSettingsScreen::turnWheel(int i)
{
    init();

	auto stereoMixerChannel = program.lock()->getPad(mpc.getPad())->getStereoMixerChannel().lock();
	auto indivFxMixerChannel = program.lock()->getPad(mpc.getPad())->getIndivFxMixerChannel().lock();

	if (param.compare("note") == 0)
	{
		int note = mpc.getNote() + i;
	
		if (note >= 35 && note <= 98)
		{
			int pad = program.lock()->getPadIndexFromNote(note);
			mpc.setPadAndNote(note, pad);
			// update(...) takes care of calling displayChannel
		}
	}
	else if (param.compare("stereovolume") == 0)
	{
		stereoMixerChannel->setLevel(stereoMixerChannel->getLevel() + i);
		displayStereoVolume();
	}
	else if (param.compare("individualvolume") == 0)
	{
		indivFxMixerChannel->setVolumeIndividualOut(indivFxMixerChannel->getVolumeIndividualOut() + i);
		displayIndividualVolume();
	}
	else if (param.compare("fxsendlevel") == 0)
	{
		indivFxMixerChannel->setFxSendLevel(indivFxMixerChannel->getFxSendLevel() + i);
		displayFxSendLevel();
	}
	else if (param.compare("panning") == 0)
	{
		stereoMixerChannel->setPanning(stereoMixerChannel->getPanning() + i);
		displayPanning();
	}
	else if (param.compare("output") == 0)
	{
		indivFxMixerChannel->setOutput(indivFxMixerChannel->getOutput() + i);
		displayOutput();
	}
	else if (param.compare("fxpath") == 0)
	{
		indivFxMixerChannel->setFxPath(indivFxMixerChannel->getFxPath() + i);
		displayFxPath();
	}
	else if (param.compare("followstereo") == 0)
	{
		indivFxMixerChannel->setFollowStereo(true);
		displayFollowStereo();
	}
}

void ChannelSettingsScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	init();

	if (s.compare("padandnote") == 0)
	{
		displayChannel();
	}
	else if (s.compare("bank") == 0)
	{
		displayChannel();
	}
}

void ChannelSettingsScreen::displayChannel()
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

void ChannelSettingsScreen::displayNoteField()
{
	string soundString = "OFF";
	auto sampleNumber = program.lock()->getNoteParameters(mpc.getNote())->getSndNumber();

	if (sampleNumber > 0 && sampleNumber < sampler.lock()->getSoundCount())
	{
		soundString = sampler.lock()->getSoundName(sampleNumber);

		if (!sampler.lock()->getSound(sampleNumber).lock()->isMono())
		{
			soundString += StrUtil::padLeft("(ST)", " ", 23 - soundString.length());
		}
	}
	findField("note").lock()->setText(to_string(mpc.getNote()) + "/" + sampler.lock()->getPadName(mpc.getPad()) + "-" + soundString);
}

void ChannelSettingsScreen::displayStereoVolume() {
	auto mixerChannel = program.lock()->getPad(mpc.getPad())->getStereoMixerChannel();
	findField("stereovolume").lock()->setTextPadded(mixerChannel.lock()->getLevel(), " ");
}

void ChannelSettingsScreen::displayIndividualVolume() {
	auto mixerChannel = program.lock()->getPad(mpc.getPad())->getIndivFxMixerChannel();
	findField("individualvolume").lock()->setTextPadded(mixerChannel.lock()->getVolumeIndividualOut(), " ");
}

void ChannelSettingsScreen::displayFxSendLevel() {
	auto mixerChannel = program.lock()->getPad(mpc.getPad())->getIndivFxMixerChannel();
	findField("fxsendlevel").lock()->setTextPadded(mixerChannel.lock()->getFxSendLevel(), " ");
}

void ChannelSettingsScreen::displayPanning()
{
	auto mixerChannel = program.lock()->getPad(mpc.getPad())->getStereoMixerChannel();

	if (mixerChannel.lock()->getPanning() != 0)
	{
		auto panning = "L";
		if (mixerChannel.lock()->getPanning() > 0)
		{
			panning = "R";
		}
		findField("panning").lock()->setText(panning + StrUtil::padLeft(to_string(abs(mixerChannel.lock()->getPanning())), " ", 2));
	}
	else
	{
		findField("panning").lock()->setText("MID");
	}
}

void ChannelSettingsScreen::displayOutput()
{
	auto indivFxMixerChannel = program.lock()->getPad(mpc.getPad())->getIndivFxMixerChannel().lock();
	auto stereoMixerChannel = program.lock()->getPad(mpc.getPad())->getStereoMixerChannel().lock();

	if (stereoMixerChannel->isStereo())
	{
		findField("output").lock()->setText(stereoNamesSlash[indivFxMixerChannel->getOutput()]);
	}
	else
	{
		findField("output").lock()->setText(" " + to_string(indivFxMixerChannel->getOutput()));
	}
}

void ChannelSettingsScreen::displayFxPath() {
	auto mixerChannel = program.lock()->getPad(mpc.getPad())->getIndivFxMixerChannel();
	findField("fxpath").lock()->setText(fxPathNames[mixerChannel.lock()->getFxPath()]);
}

void ChannelSettingsScreen::displayFollowStereo() {
	auto mixerScreen = dynamic_pointer_cast<MixerScreen>(Screens::getScreenComponent("mixer"));
	auto mixerChannel = program.lock()->getPad(mixerScreen->getXPos() + (mpc.getBank() * 16))->getIndivFxMixerChannel();
	findField("followstereo").lock()->setText(mixerChannel.lock()->isFollowingStereo() ? "YES" : "NO");
}
