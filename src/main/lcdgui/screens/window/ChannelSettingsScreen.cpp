#include "ChannelSettingsScreen.hpp"

#include <sampler/Pad.hpp>

#include <lcdgui/screens/MixerSetupScreen.hpp>
#include <lcdgui/screens/MixerScreen.hpp>

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;
using namespace mpc::engine;

ChannelSettingsScreen::ChannelSettingsScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "channel-settings", layerIndex)
{
}

void ChannelSettingsScreen::open()
{
	init();
	auto mixerScreen = mpc.screens->get<MixerScreen>();
    const int padIndexWithoutBank = mixerScreen->xPos;
    const int padIndexWithBank = padIndexWithoutBank + (mpc.getBank() * 16);
    auto padNote = program->getNoteFromPad(padIndexWithBank);
	note = padNote == 34 ? 35 : padNote;
	displayChannel();
	mpc.addObserver(this);
}

void ChannelSettingsScreen::close()
{
	mpc.deleteObserver(this);
}

std::shared_ptr<IndivFxMixer> ChannelSettingsScreen::getIndivFxMixerChannel()
{
	init();

	auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>();

	if (mixerSetupScreen->isIndivFxSourceDrum())
	{
		return activeDrum().getIndivFxMixerChannels()[note - 35];
	}
	else
	{
		auto noteParameters = dynamic_cast<NoteParameters*>(program->getNoteParameters(note));
		return noteParameters->getIndivFxMixerChannel();
	}
}

std::shared_ptr<StereoMixer> ChannelSettingsScreen::getStereoMixerChannel()
{
	init();

	auto mixerSetupScreen = mpc.screens->get<MixerSetupScreen>();

	if (mixerSetupScreen->isStereoMixSourceDrum())
	{
		return activeDrum().getStereoMixerChannels()[note - 35];
	}
	else
	{
		auto noteParameters = dynamic_cast<NoteParameters*>(program->getNoteParameters(note));
		return noteParameters->getStereoMixerChannel();
	}
}

void ChannelSettingsScreen::turnWheel(int i)
{
    init();

	auto stereoMixerChannel = getStereoMixerChannel();
	auto indivFxMixerChannel = getIndivFxMixerChannel();

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

	if (focusedFieldName == "note")
	{
		setNote(note + i);
	}
	else if (focusedFieldName == "stereovolume")
	{
		stereoMixerChannel->setLevel(stereoMixerChannel->getLevel() + i);
		displayStereoVolume();
	}
	else if (focusedFieldName == "individualvolume")
	{
		indivFxMixerChannel->setVolumeIndividualOut(indivFxMixerChannel->getVolumeIndividualOut() + i);
		displayIndividualVolume();
	}
	else if (focusedFieldName == "fxsendlevel")
	{
		indivFxMixerChannel->setFxSendLevel(indivFxMixerChannel->getFxSendLevel() + i);
		displayFxSendLevel();
	}
	else if (focusedFieldName == "panning")
	{
		stereoMixerChannel->setPanning(stereoMixerChannel->getPanning() + i);
		displayPanning();
	}
	else if (focusedFieldName == "output")
	{
		indivFxMixerChannel->setOutput(indivFxMixerChannel->getOutput() + i);
		displayOutput();
	}
	else if (focusedFieldName == "fxpath")
	{
		indivFxMixerChannel->setFxPath(indivFxMixerChannel->getFxPath() + i);
		displayFxPath();
	}
	else if (focusedFieldName == "followstereo")
	{
		indivFxMixerChannel->setFollowStereo(true);
		displayFollowStereo();
	}
}

void ChannelSettingsScreen::update(Observable* o, Message message)
{
	init();

    const auto msg = std::get<std::string>(message);

	if (msg == "note")
    {
        setNote(mpc.getNote());
    }
	else if (msg == "bank")
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
    std::string soundName = "OFF";
	auto soundIndex = program->getNoteParameters(note)->getSoundIndex();

	if (soundIndex >= 0 && soundIndex < sampler->getSoundCount())
	{
		soundName = sampler->getSoundName(soundIndex);

		if (!sampler->getSound(soundIndex)->isMono())
			soundName += StrUtil::padLeft("(ST)", " ", 19 - soundName.length());
	}

    auto padIndex = program->getPadIndexFromNote(note);
    auto padName = sampler->getPadName(padIndex);
    findField("note")->setText(std::to_string(note) + "/" + padName + "-" + soundName);
}

void ChannelSettingsScreen::displayStereoVolume()
{
    auto noteParameters = dynamic_cast<NoteParameters*>(program->getNoteParameters(note));
    auto mixerChannel = noteParameters->getStereoMixerChannel();
    findField("stereovolume")->setTextPadded(mixerChannel->getLevel(), " ");
}

void ChannelSettingsScreen::displayIndividualVolume()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program->getNoteParameters(note));
    auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("individualvolume")->setTextPadded(mixerChannel->getVolumeIndividualOut(), " ");
}

void ChannelSettingsScreen::displayFxSendLevel()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program->getNoteParameters(note));
    auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("fxsendlevel")->setTextPadded(mixerChannel->getFxSendLevel(), " ");
}

void ChannelSettingsScreen::displayPanning()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program->getNoteParameters(note));
    auto mixerChannel = noteParameters->getStereoMixerChannel();

    const int8_t normalizedPan = mixerChannel->getPanning() - 50;

    if (normalizedPan == 0)
    {
        findField("panning")->setText("MID");
        return;
    }

    auto panning = "L";

    if (normalizedPan > 0)
    {
        panning = "R";
    }

    findField("panning")->setText(
        panning + StrUtil::padLeft(std::to_string(abs(normalizedPan)), " ", 2));
}

void ChannelSettingsScreen::displayOutput()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program->getNoteParameters(note));
    auto indivFxMixerChannel = noteParameters->getIndivFxMixerChannel();
    auto stereoMixerChannel = noteParameters->getStereoMixerChannel();
    auto soundIndex = noteParameters->getSoundIndex();
    const bool isStereo = soundIndex != -1 && !sampler->getSound(soundIndex)->isMono();

    if (isStereo)
        findField("output")->setText(stereoNamesSlash[indivFxMixerChannel->getOutput()]);
    else
        findField("output")->setText(" " + std::to_string(indivFxMixerChannel->getOutput()));
}

void ChannelSettingsScreen::displayFxPath()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program->getNoteParameters(note));
    auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("fxpath")->setText(fxPathNames[mixerChannel->getFxPath()]);
}

void ChannelSettingsScreen::displayFollowStereo()
{
    auto noteParameters = dynamic_cast<NoteParameters *>(program->getNoteParameters(note));
    auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("followstereo")->setText(mixerChannel->isFollowingStereo() ? "YES" : "NO");
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
