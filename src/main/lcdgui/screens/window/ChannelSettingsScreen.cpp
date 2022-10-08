#include "ChannelSettingsScreen.hpp"

#include <sampler/Pad.hpp>

#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/MixerScreen.hpp>

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
	auto mixerScreen = mpc.screens->get<MixerScreen>("mixer");
    const int padIndexWithoutBank = mixerScreen->xPos;
    const int padIndexWithBank = padIndexWithoutBank + (mpc.getBank() * 16);
    auto padNote = program.lock()->getNoteFromPad(padIndexWithBank);
	note = padNote == 34 ? 35 : padNote;
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

	auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");

	if (mixerSetupScreen->isIndivFxSourceDrum())
	{
		return mpcSoundPlayerChannel->getIndivFxMixerChannels()[note - 35];
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

	auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>("mixer-setup");

	if (mixerSetupScreen->isStereoMixSourceDrum())
	{
		return mpcSoundPlayerChannel->getStereoMixerChannels()[note - 35];
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

	auto stereoMixerChannel = getStereoMixerChannel().lock();
	auto indivFxMixerChannel = getIndivFxMixerChannel().lock();

	if (param == "note")
	{
		setNote(note + i);
	}
	else if (param == "stereovolume")
	{
		stereoMixerChannel->setLevel(stereoMixerChannel->getLevel() + i);
		displayStereoVolume();
	}
	else if (param == "individualvolume")
	{
		indivFxMixerChannel->setVolumeIndividualOut(indivFxMixerChannel->getVolumeIndividualOut() + i);
		displayIndividualVolume();
	}
	else if (param == "fxsendlevel")
	{
		indivFxMixerChannel->setFxSendLevel(indivFxMixerChannel->getFxSendLevel() + i);
		displayFxSendLevel();
	}
	else if (param == "panning")
	{
		stereoMixerChannel->setPanning(stereoMixerChannel->getPanning() + i);
		displayPanning();
	}
	else if (param == "output")
	{
		indivFxMixerChannel->setOutput(indivFxMixerChannel->getOutput() + i);
		displayOutput();
	}
	else if (param == "fxpath")
	{
		indivFxMixerChannel->setFxPath(indivFxMixerChannel->getFxPath() + i);
		displayFxPath();
	}
	else if (param == "followstereo")
	{
		indivFxMixerChannel->setFollowStereo(true);
		displayFollowStereo();
	}
}

void ChannelSettingsScreen::update(moduru::observer::Observable* o, nonstd::any arg)
{
	string s = nonstd::any_cast<string>(arg);

	init();

	if (s == "note")
    {
        setNote(mpc.getNote());
    }
	else if (s == "bank")
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
	auto soundIndex = program.lock()->getNoteParameters(note)->getSoundIndex();

	if (soundIndex >= 0 && soundIndex < sampler.lock()->getSoundCount())
	{
		soundName = sampler.lock()->getSoundName(soundIndex);

		if (!sampler.lock()->getSound(soundIndex).lock()->isMono())
			soundName += StrUtil::padLeft("(ST)", " ", 19 - soundName.length());
	}

    auto padIndex = program.lock()->getPadIndexFromNote(note);
    auto padName = sampler.lock()->getPadName(padIndex);
    findField("note").lock()->setText(to_string(note) + "/" + padName + "-" + soundName);
}

void ChannelSettingsScreen::displayStereoVolume()
{
    auto noteParameters = dynamic_cast<NoteParameters*>(program.lock()->getNoteParameters(note));
    auto mixerChannel = noteParameters->getStereoMixerChannel();
    findField("stereovolume").lock()->setTextPadded(mixerChannel.lock()->getLevel(), " ");
}

void ChannelSettingsScreen::displayIndividualVolume()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program.lock()->getNoteParameters(note));
    auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("individualvolume").lock()->setTextPadded(mixerChannel.lock()->getVolumeIndividualOut(), " ");
}

void ChannelSettingsScreen::displayFxSendLevel()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program.lock()->getNoteParameters(note));
    auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("fxsendlevel").lock()->setTextPadded(mixerChannel.lock()->getFxSendLevel(), " ");
}

void ChannelSettingsScreen::displayPanning()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program.lock()->getNoteParameters(note));
    auto mixerChannel = noteParameters->getStereoMixerChannel();

    if (mixerChannel.lock()->getPanning() != 0) {
        auto panning = "L";

        if (mixerChannel.lock()->getPanning() > 0)
            panning = "R";

        findField("panning").lock()->setText(
                panning + StrUtil::padLeft(to_string(abs(mixerChannel.lock()->getPanning())), " ", 2));
    } else {
        findField("panning").lock()->setText("MID");
    }
}

void ChannelSettingsScreen::displayOutput()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program.lock()->getNoteParameters(note));
    auto indivFxMixerChannel = noteParameters->getIndivFxMixerChannel().lock();
    auto stereoMixerChannel = noteParameters->getStereoMixerChannel().lock();

    if (stereoMixerChannel->isStereo())
        findField("output").lock()->setText(stereoNamesSlash[indivFxMixerChannel->getOutput()]);
    else
        findField("output").lock()->setText(" " + to_string(indivFxMixerChannel->getOutput()));
}

void ChannelSettingsScreen::displayFxPath()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program.lock()->getNoteParameters(note));
    auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("fxpath").lock()->setText(fxPathNames[mixerChannel.lock()->getFxPath()]);
}

void ChannelSettingsScreen::displayFollowStereo()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program.lock()->getNoteParameters(note));
    auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("followstereo").lock()->setText(mixerChannel.lock()->isFollowingStereo() ? "YES" : "NO");
}

void ChannelSettingsScreen::setNote(int newNote)
{
	if (newNote < 35 || newNote > 98)
    {
        return;
    }

	this->note = newNote;
	displayChannel();
}
