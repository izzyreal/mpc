#include "ChannelSettingsScreen.hpp"

#include <sampler/Pad.hpp>

#include <lcdgui/screens/MixerScreen.hpp>
#include <lcdgui/screens/MixerSetupScreen.hpp>

#include <mpc/MpcSoundPlayerChannel.hpp>
#include <mpc/MpcStereoMixerChannel.hpp>
#include <mpc/MpcIndivFxMixerChannel.hpp>

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;
using namespace ctoot::mpc;
using namespace moduru::lang;
using namespace std;

ChannelSettingsScreen::ChannelSettingsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "channel-settings", layerIndex)
{
}

void ChannelSettingsScreen::open()
{
	init();
	const auto note_ = program.lock()->getNoteFromPad(mpc.getPad());

	note = note_ == 34 ? 35 : note_;
	displayChannel();
	mpc.addObserver(this);
}

void ChannelSettingsScreen::close()
{
	mpc.deleteObserver(this);
}

weak_ptr<MpcIndivFxMixerChannel> ChannelSettingsScreen::getIndivFxMixerChannel()
{
	init();
	
	if (note == 34)
		return {};

	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));

	if (mixerSetupScreen->isIndivFxSourceDrum())
	{
		auto padIndex = program.lock()->getPadIndexFromNote(note);
		
		if (padIndex == -1)
			return {};
		
		return mpcSoundPlayerChannel->getIndivFxMixerChannels()[padIndex];
	}
	else
	{
		auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
		return noteParameters->getIndivFxMixerChannel().lock();
	}
}

weak_ptr<MpcStereoMixerChannel> ChannelSettingsScreen::getStereoMixerChannel()
{
	init();
	
	if (note == 34)
		return {};

	auto mixerSetupScreen = dynamic_pointer_cast<MixerSetupScreen>(mpc.screens->getScreenComponent("mixer-setup"));

	if (mixerSetupScreen->isStereoMixSourceDrum())
	{
		auto padIndex = program.lock()->getPadIndexFromNote(note);
		
		if (padIndex == -1)
			return {};
		
		return mpcSoundPlayerChannel->getStereoMixerChannels()[padIndex];
	}
	else
	{
		auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
		return noteParameters->getStereoMixerChannel().lock();
	}
}

void ChannelSettingsScreen::turnWheel(int i)
{
    init();

	auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
	auto stereoMixerChannel = getStereoMixerChannel().lock();
	auto indivFxMixerChannel = getIndivFxMixerChannel().lock();

	if (param.compare("note") == 0)
	{
		setNote(note + i);
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
		if (mpc.getNote() != 34)
			setNote(mpc.getNote());
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
	string soundName = "OFF";
	auto soundIndex = program.lock()->getNoteParameters(note)->getSndNumber();

	if (soundIndex >= 0 && soundIndex < sampler.lock()->getSoundCount())
	{
		soundName = sampler.lock()->getSoundName(soundIndex);

		if (!sampler.lock()->getSound(soundIndex).lock()->isMono())
			soundName += StrUtil::padLeft("(ST)", " ", 23 - soundName.length());
	}

	if (note == 34)
	{
		findField("note").lock()->setText("");
	}
	else
	{
		auto padIndex = program.lock()->getPadIndexFromNote(note);
		findField("note").lock()->setText(to_string(note) + "/" + sampler.lock()->getPadName(padIndex) + "-" + soundName);
	}
}

void ChannelSettingsScreen::displayStereoVolume()
{
	auto note = program.lock()->getNoteFromPad(mpc.getPad());
	if (note == 34)
	{
		findField("stereovolume").lock()->setText("");
	}
	else {
		auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
		auto mixerChannel = noteParameters->getStereoMixerChannel();
		findField("stereovolume").lock()->setTextPadded(mixerChannel.lock()->getLevel(), " ");
	}
}

void ChannelSettingsScreen::displayIndividualVolume()
{
	auto note = program.lock()->getNoteFromPad(mpc.getPad());
	if (note == 34)
	{
		findField("individualvolume").lock()->setText("");
	}
	else {
		auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
		auto mixerChannel = noteParameters->getIndivFxMixerChannel();
		findField("individualvolume").lock()->setTextPadded(mixerChannel.lock()->getVolumeIndividualOut(), " ");
	}
}

void ChannelSettingsScreen::displayFxSendLevel()
{
	auto note = program.lock()->getNoteFromPad(mpc.getPad());
	if (note == 34)
	{
		findField("fxsendlevel").lock()->setText("");
	}
	else {
		auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
		auto mixerChannel = noteParameters->getIndivFxMixerChannel();
		findField("fxsendlevel").lock()->setTextPadded(mixerChannel.lock()->getFxSendLevel(), " ");
	}
}

void ChannelSettingsScreen::displayPanning()
{
	auto note = program.lock()->getNoteFromPad(mpc.getPad());
	if (note == 34)
	{
		findField("panning").lock()->setText("");
	}
	else {
		auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
		auto mixerChannel = noteParameters->getStereoMixerChannel();

		if (mixerChannel.lock()->getPanning() != 0)
		{
			auto panning = "L";

			if (mixerChannel.lock()->getPanning() > 0)
				panning = "R";

			findField("panning").lock()->setText(panning + StrUtil::padLeft(to_string(abs(mixerChannel.lock()->getPanning())), " ", 2));
		}
		else
		{
			findField("panning").lock()->setText("MID");
		}
	}
}

void ChannelSettingsScreen::displayOutput()
{
	auto note = program.lock()->getNoteFromPad(mpc.getPad());
	if (note == 34)
	{
		findField("output").lock()->setText("");
	}
	else {
		auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
		auto indivFxMixerChannel = noteParameters->getIndivFxMixerChannel().lock();
		auto stereoMixerChannel = noteParameters->getStereoMixerChannel().lock();

		if (stereoMixerChannel->isStereo())
			findField("output").lock()->setText(stereoNamesSlash[indivFxMixerChannel->getOutput()]);
		else
			findField("output").lock()->setText(" " + to_string(indivFxMixerChannel->getOutput()));
	}
}

void ChannelSettingsScreen::displayFxPath()
{
	auto note = program.lock()->getNoteFromPad(mpc.getPad());

	if (note == 34)
	{
		findField("fxpath").lock()->setText("");
	}
	else {
		auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
		auto mixerChannel = noteParameters->getIndivFxMixerChannel();
		findField("fxpath").lock()->setText(fxPathNames[mixerChannel.lock()->getFxPath()]);
	}
}

void ChannelSettingsScreen::displayFollowStereo() 
{
	auto note = program.lock()->getNoteFromPad(mpc.getPad());
	if (note == 34)
	{
		findField("followstereo").lock()->setText("");
	}
	else {
		auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
		auto mixerChannel = noteParameters->getIndivFxMixerChannel();
		findField("followstereo").lock()->setText(mixerChannel.lock()->isFollowingStereo() ? "YES" : "NO");
	}
}

void ChannelSettingsScreen::setNote(int note)
{
	if (note < 35 || note > 98)
		return;

	this->note = note;
	displayChannel();
}
