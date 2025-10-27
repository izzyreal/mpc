#include "MetronomeSoundScreen.hpp"
#include "controller/ClientEventController.hpp"
#include "sampler/Pad.hpp"

#include <Mpc.hpp>

#include "lcdgui/Background.hpp"
#include "sampler/Program.hpp"

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;

MetronomeSoundScreen::MetronomeSoundScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "metronome-sound", layerIndex)
{
    sampler = mpc.getSampler();
}

void MetronomeSoundScreen::open()
{
    // There was an issue with previous ALL parsers, which resulted in previous
    // demo data versions to have erroneous ALL files that crashed this screen
    // because of out of range pads. Here we make sure that doesn't
    // happen anymore. Also the demo data has been fixed, but once the user has
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

void MetronomeSoundScreen::displaySound()
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

void MetronomeSoundScreen::displayVolume()
{
    findField("volume")->setTextPadded(volume);
}

void MetronomeSoundScreen::displayOutput()
{
    auto outputText = output == 0 ? "STEREO" : "OUT" + std::to_string(output);
    findField("output")->setText(outputText);
}

void MetronomeSoundScreen::displayAccent()
{
    auto program = sampler->getDrumBusProgramIndex(sound);
    auto note = dynamic_cast<Program *>(sampler->getProgram(program).get())
                    ->getPad(accentPad)
                    ->getNote();
    findField("accent")->setText((note == 34 ? "--" : std::to_string(note)) +
                                 "/" + sampler->getPadName(accentPad));
}

void MetronomeSoundScreen::displayNormal()
{
    auto program = sampler->getDrumBusProgramIndex(sound);
    auto note = dynamic_cast<Program *>(sampler->getProgram(program).get())
                    ->getPad(normalPad)
                    ->getNote();
    findField("normal")->setText((note == 34 ? "--" : std::to_string(note)) +
                                 "/" + sampler->getPadName(normalPad));
}

void MetronomeSoundScreen::displayAccentVelo()
{
    findField("velocity-accent")->setTextPadded(accentVelo);
}

void MetronomeSoundScreen::displayNormalVelo()
{
    findField("velocity-normal")->setTextPadded(normalVelo);
}

int MetronomeSoundScreen::getVolume()
{
    return volume;
}

void MetronomeSoundScreen::setVolume(int i)
{
    volume = std::clamp(i, 0, 100);
    displayVolume();
}

int MetronomeSoundScreen::getOutput()
{
    return output;
}

void MetronomeSoundScreen::setOutput(int i)
{
    output = std::clamp(i, 0, 8);
    displayOutput();
}

int MetronomeSoundScreen::getSound()
{
    return sound;
}

void MetronomeSoundScreen::setSound(int i)
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

int MetronomeSoundScreen::getAccentPad()
{
    return accentPad;
}

void MetronomeSoundScreen::setAccentPad(int i)
{
    accentPad = std::clamp(i, 0, 63);
    displayAccent();
}

int MetronomeSoundScreen::getAccentVelo()
{
    return accentVelo;
}

void MetronomeSoundScreen::setAccentVelo(int i)
{
    accentVelo = std::clamp(i, 1, 127);
    displayAccentVelo();
}

int MetronomeSoundScreen::getNormalPad()
{
    return normalPad;
}

void MetronomeSoundScreen::setNormalPad(int i)
{
    normalPad = std::clamp(i, 0, 63);
    displayNormal();
}

int MetronomeSoundScreen::getNormalVelo()
{
    return normalVelo;
}

void MetronomeSoundScreen::setNormalVelo(int i)
{
    normalVelo = std::clamp(i, 1, 127);
    displayNormalVelo();
}

void MetronomeSoundScreen::function(int i)
{
    switch (i)
    {
        case 3:
            mpc.getLayeredScreen()->openScreen<CountMetronomeScreen>();
            break;
    }
}

void MetronomeSoundScreen::turnWheel(int i)
{
    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "sound")
    {
        setSound(sound + i);
    }
    else if (focusedFieldName == "volume")
    {
        setVolume(volume + i);
    }
    else if (focusedFieldName == "output")
    {
        setOutput(output + i);
    }
    else if (focusedFieldName == "accent")
    {
        setAccentPad(accentPad + i);
    }
    else if (focusedFieldName == "normal")
    {
        setNormalPad(normalPad + i);
    }
    else if (focusedFieldName == "velocity-accent")
    {
        setAccentVelo(accentVelo + i);
    }
    else if (focusedFieldName == "velocity-normal")
    {
        setNormalVelo(normalVelo + i);
    }
}

void MetronomeSoundScreen::update(Observable *o, Message message)
{
    const auto msg = std::get<std::string>(message);

    if (msg == "note")
    {
        const auto focusedFieldName = getFocusedFieldNameOrThrow();

        if (focusedFieldName == "accent")
        {
            setAccentPad(mpc.clientEventController->getSelectedPad());
        }
        else if (focusedFieldName == "normal")
        {
            setNormalPad(mpc.clientEventController->getSelectedPad());
        }
    }
}
