#include "ChannelSettingsScreen.hpp"

#include "Mpc.hpp"
#include "StrUtil.hpp"
#include "controller/ClientEventController.hpp"

#include "lcdgui/screens/MixerSetupScreen.hpp"
#include "lcdgui/screens/MixerScreen.hpp"

#include "engine/StereoMixer.hpp"
#include "engine/IndivFxMixer.hpp"
#include "sampler/Program.hpp"
#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

using namespace mpc::lcdgui::screens::window;
using namespace mpc::sampler;
using namespace mpc::engine;

ChannelSettingsScreen::ChannelSettingsScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "channel-settings", layerIndex)
{
}

void ChannelSettingsScreen::open()
{
    const auto bank = mpc.clientEventController->getActiveBank();
    const auto mixerScreen = mpc.screens->get<ScreenId::MixerScreen>();
    const auto padIndexWithoutBank = PhysicalPadIndex(mixerScreen->xPos);
    const auto padIndexWithBank =
        controller::physicalPadAndBankToProgramPadIndex(padIndexWithoutBank,
                                                        bank);
    const auto padNote = getProgramOrThrow()->getNoteFromPad(padIndexWithBank);
    note = padNote == NoDrumNoteAssigned ? MinDrumNoteNumber : padNote;
    displayChannel();
    mpc.clientEventController->addObserver(this);
}

void ChannelSettingsScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

std::shared_ptr<IndivFxMixer>
ChannelSettingsScreen::getIndivFxMixerChannel() const
{
    const auto mixerSetupScreen =
        mpc.screens->get<ScreenId::MixerSetupScreen>();

    if (mixerSetupScreen->isIndivFxSourceDrum())
    {
        return getActiveDrumBus()->getIndivFxMixerChannels()[note - 35];
    }
    const auto noteParameters = getProgramOrThrow()->getNoteParameters(note);
    return noteParameters->getIndivFxMixerChannel();
}

std::shared_ptr<StereoMixer>
ChannelSettingsScreen::getStereoMixerChannel() const
{
    const auto mixerSetupScreen =
        mpc.screens->get<ScreenId::MixerSetupScreen>();

    if (mixerSetupScreen->isStereoMixSourceDrum())
    {
        return getActiveDrumBus()->getStereoMixerChannels()[note - 35];
    }
    const auto noteParameters = getProgramOrThrow()->getNoteParameters(note);
    return noteParameters->getStereoMixerChannel();
}

void ChannelSettingsScreen::turnWheel(const int increment)
{
    const auto stereoMixerChannel = getStereoMixerChannel();
    const auto indivFxMixerChannel = getIndivFxMixerChannel();

    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "note")
    {
        setNote(note + increment);
    }
    else if (focusedFieldName == "stereovolume")
    {
        stereoMixerChannel->setLevel(stereoMixerChannel->getLevel() +
                                     increment);
        displayStereoVolume();
    }
    else if (focusedFieldName == "individualvolume")
    {
        indivFxMixerChannel->setVolumeIndividualOut(
            indivFxMixerChannel->getVolumeIndividualOut() + increment);
        displayIndividualVolume();
    }
    else if (focusedFieldName == "fxsendlevel")
    {
        indivFxMixerChannel->setFxSendLevel(
            indivFxMixerChannel->getFxSendLevel() + increment);
        displayFxSendLevel();
    }
    else if (focusedFieldName == "panning")
    {
        stereoMixerChannel->setPanning(stereoMixerChannel->getPanning() +
                                       increment);
        displayPanning();
    }
    else if (focusedFieldName == "output")
    {
        indivFxMixerChannel->setOutput(indivFxMixerChannel->getOutput() +
                                       increment);
        displayOutput();
    }
    else if (focusedFieldName == "fxpath")
    {
        indivFxMixerChannel->setFxPath(indivFxMixerChannel->getFxPath() +
                                       increment);
        displayFxPath();
    }
    else if (focusedFieldName == "followstereo")
    {
        indivFxMixerChannel->setFollowStereo(true);
        displayFollowStereo();
    }
}

void ChannelSettingsScreen::update(Observable *observable,
                                   const Message message)
{
    if (const auto msg = std::get<std::string>(message); msg == "note")
    {
        setNote(mpc.clientEventController->getSelectedNote());
    }
    else if (msg == "bank")
    {
        displayChannel();
    }
}

void ChannelSettingsScreen::displayChannel() const
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

void ChannelSettingsScreen::displayNoteField() const
{
    std::string soundName = "OFF";
    const auto program = getProgramOrThrow();

    if (const auto soundIndex =
            program->getNoteParameters(note)->getSoundIndex();
        soundIndex >= 0 && soundIndex < sampler->getSoundCount())
    {
        soundName = sampler->getSoundName(soundIndex);

        if (!sampler->getSound(soundIndex)->isMono())
        {
            soundName += StrUtil::padLeft("(ST)", " ", 19 - soundName.length());
        }
    }

    const auto padIndex = program->getPadIndexFromNote(note);
    const auto padName = sampler->getPadName(padIndex);
    findField("note")->setText(std::to_string(note) + "/" + padName + "-" +
                               soundName);
}

void ChannelSettingsScreen::displayStereoVolume() const
{
    const auto program = getProgramOrThrow();
    const auto noteParameters = program->getNoteParameters(note);
    const auto mixerChannel = noteParameters->getStereoMixerChannel();
    findField("stereovolume")->setTextPadded(mixerChannel->getLevel(), " ");
}

void ChannelSettingsScreen::displayIndividualVolume() const
{
    const auto program = getProgramOrThrow();
    const auto noteParameters = program->getNoteParameters(note);
    const auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("individualvolume")
        ->setTextPadded(mixerChannel->getVolumeIndividualOut(), " ");
}

void ChannelSettingsScreen::displayFxSendLevel() const
{
    const auto program = getProgramOrThrow();
    const auto noteParameters = program->getNoteParameters(note);
    const auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("fxsendlevel")
        ->setTextPadded(mixerChannel->getFxSendLevel(), " ");
}

void ChannelSettingsScreen::displayPanning() const
{
    const auto program = getProgramOrThrow();
    const auto noteParameters = program->getNoteParameters(note);
    const auto mixerChannel = noteParameters->getStereoMixerChannel();

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

    const auto panningText =
        panning + StrUtil::padLeft(std::to_string(abs(normalizedPan)), " ", 2);

    findField("panning")->setText(panningText);
}

void ChannelSettingsScreen::displayOutput() const
{
    const auto program = getProgramOrThrow();
    const auto noteParameters = program->getNoteParameters(note);
    const auto indivFxMixerChannel = noteParameters->getIndivFxMixerChannel();
    auto stereoMixerChannel = noteParameters->getStereoMixerChannel();
    const auto soundIndex = noteParameters->getSoundIndex();
    const bool isStereo =
        soundIndex != -1 && !sampler->getSound(soundIndex)->isMono();

    if (isStereo)
    {
        findField("output")->setText(
            stereoNamesSlash[indivFxMixerChannel->getOutput()]);
    }
    else
    {
        findField("output")->setText(
            " " + std::to_string(indivFxMixerChannel->getOutput()));
    }
}

void ChannelSettingsScreen::displayFxPath() const
{
    const auto program = getProgramOrThrow();
    const auto noteParameters = program->getNoteParameters(note);
    const auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("fxpath")->setText(fxPathNames[mixerChannel->getFxPath()]);
}

void ChannelSettingsScreen::displayFollowStereo() const
{
    const auto program = getProgramOrThrow();
    const auto noteParameters = program->getNoteParameters(note);
    const auto mixerChannel = noteParameters->getIndivFxMixerChannel();
    findField("followstereo")
        ->setText(mixerChannel->isFollowingStereo() ? "YES" : "NO");
}

void ChannelSettingsScreen::setNote(const DrumNoteNumber newNote)
{
    note = std::clamp(newNote, MinDrumNoteNumber, MaxDrumNoteNumber);
    displayChannel();
}
