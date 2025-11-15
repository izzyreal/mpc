#include "MetronomeSoundScreen.hpp"
#include "controller/ClientEventController.hpp"
#include "sampler/Pad.hpp"

#include "sampler/Sampler.hpp"
#include "sequencer/Bus.hpp"

#include "Mpc.hpp"

#include "lcdgui/Background.hpp"
#include "lcdgui/Label.hpp"
#include "sampler/Program.hpp"
#include "sequencer/Sequencer.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;
using namespace mpc::sequencer;

MetronomeSoundScreen::MetronomeSoundScreen(Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "metronome-sound", layerIndex)
{
    sampler = mpc.getSampler();
}

void MetronomeSoundScreen::open()
{
    // There was an issue with previous ALL parsers, which resulted in previous
    // demo data versions to have erroneous ALL files that crashed this screen
    // because of out of range pads. Here we make sure that doesn't
    // happen anymore. Also, the demo data has been fixed, but once the user has
    // loaded an ALL file, this erroneous setting may have persisted into their
    // files too.
    if (accentPad < 0 || accentPad > 63)
    {
        accentPad = 0;
    }
    if (normalPad < 0 || normalPad > 63)
    {
        normalPad = 0;
    }

    setSound(sound);
    mpc.clientEventController->addObserver(
        this); // Subscribe to "note" messages
}

void MetronomeSoundScreen::close()
{
    mpc.clientEventController->deleteObserver(this);
}

void MetronomeSoundScreen::displaySound() const
{
    findField("sound")->setText(soundNames[sound]);
    findLabel("volume")->Hide(sound != 0);
    findField("volume")->Hide(sound != 0);
    findLabel("output")->Hide(sound != 0);
    findField("output")->Hide(sound != 0);
    findLabel("normal")->Hide(sound == 0);
    findField("normal")->Hide(sound == 0);
    findLabel("velocity-normal")->Hide(sound == 0);
    findField("velocity-normal")->Hide(sound == 0);
    findLabel("accent")->Hide(sound == 0);
    findField("accent")->Hide(sound == 0);
    findLabel("velocity-accent")->Hide(sound == 0);
    findField("velocity-accent")->Hide(sound == 0);
}

void MetronomeSoundScreen::displayVolume() const
{
    findField("volume")->setTextPadded(volume);
}

void MetronomeSoundScreen::displayOutput() const
{
    const auto outputText =
        output == 0 ? "STEREO" : "OUT" + std::to_string(output);
    findField("output")->setText(outputText);
}

void MetronomeSoundScreen::displayAccent() const
{
    const auto drumBus = sequencer->getBus<DrumBus>(busIndexToBusType(sound));
    const auto program = drumBus->getProgramIndex();
    const auto note =
        sampler->getProgram(program)->getPad(accentPad)->getNote();
    findField("accent")->setText((note == 34 ? "--" : std::to_string(note)) +
                                 "/" + sampler->getPadName(accentPad));
}

void MetronomeSoundScreen::displayNormal() const
{
    const auto drumBus = sequencer->getBus<DrumBus>(busIndexToBusType(sound));
    const auto program = drumBus->getProgramIndex();
    const auto note =
        sampler->getProgram(program)->getPad(normalPad)->getNote();
    findField("normal")->setText((note == 34 ? "--" : std::to_string(note)) +
                                 "/" + sampler->getPadName(normalPad));
}

void MetronomeSoundScreen::displayAccentVelo() const
{
    findField("velocity-accent")->setTextPadded(accentVelo);
}

void MetronomeSoundScreen::displayNormalVelo() const
{
    findField("velocity-normal")->setTextPadded(normalVelo);
}

int MetronomeSoundScreen::getVolume() const
{
    return volume;
}

void MetronomeSoundScreen::setVolume(const int i)
{
    volume = std::clamp(i, 0, 100);
    displayVolume();
}

int MetronomeSoundScreen::getOutput() const
{
    return output;
}

void MetronomeSoundScreen::setOutput(const int i)
{
    output = std::clamp(i, 0, 8);
    displayOutput();
}

int MetronomeSoundScreen::getSound() const
{
    return sound;
}

void MetronomeSoundScreen::setSound(const int i)
{
    sound = std::clamp(i, 0, 4);
    displaySound();

    if (sound == 0)
    {
        displayVolume();
        displayOutput();
        findBackground()->setBackgroundName("metronome-sound");
    }
    else
    {
        displayAccent();
        displayNormal();
        displayAccentVelo();
        displayNormalVelo();
        findBackground()->setBackgroundName("metronome-sound-empty");
    }
}

int MetronomeSoundScreen::getAccentPad() const
{
    return accentPad;
}

void MetronomeSoundScreen::setAccentPad(const int i)
{
    accentPad = std::clamp(i, 0, 63);
    displayAccent();
}

int MetronomeSoundScreen::getAccentVelo() const
{
    return accentVelo;
}

void MetronomeSoundScreen::setAccentVelo(const int i)
{
    accentVelo = std::clamp(i, 1, 127);
    displayAccentVelo();
}

int MetronomeSoundScreen::getNormalPad() const
{
    return normalPad;
}

void MetronomeSoundScreen::setNormalPad(const int i)
{
    normalPad = std::clamp(i, 0, 63);
    displayNormal();
}

int MetronomeSoundScreen::getNormalVelo() const
{
    return normalVelo;
}

void MetronomeSoundScreen::setNormalVelo(const int i)
{
    normalVelo = std::clamp(i, 1, 127);
    displayNormalVelo();
}

void MetronomeSoundScreen::function(const int i)
{
    switch (i)
    {
        case 3:
            openScreenById(ScreenId::CountMetronomeScreen);
            break;
        default:;
    }
}

void MetronomeSoundScreen::turnWheel(const int increment)
{
    if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
        focusedFieldName == "sound")
    {
        setSound(sound + increment);
    }
    else if (focusedFieldName == "volume")
    {
        setVolume(volume + increment);
    }
    else if (focusedFieldName == "output")
    {
        setOutput(output + increment);
    }
    else if (focusedFieldName == "accent")
    {
        setAccentPad(accentPad + increment);
    }
    else if (focusedFieldName == "normal")
    {
        setNormalPad(normalPad + increment);
    }
    else if (focusedFieldName == "velocity-accent")
    {
        setAccentVelo(accentVelo + increment);
    }
    else if (focusedFieldName == "velocity-normal")
    {
        setNormalVelo(normalVelo + increment);
    }
}

void MetronomeSoundScreen::update(Observable *o, const Message message)
{
    if (const auto msg = std::get<std::string>(message); msg == "note")
    {
        if (const auto focusedFieldName = getFocusedFieldNameOrThrow();
            focusedFieldName == "accent")
        {
            setAccentPad(mpc.clientEventController->getSelectedPad());
        }
        else if (focusedFieldName == "normal")
        {
            setNormalPad(mpc.clientEventController->getSelectedPad());
        }
    }
}
