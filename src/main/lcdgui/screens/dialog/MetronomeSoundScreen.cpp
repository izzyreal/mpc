#include "MetronomeSoundScreen.hpp"
#include "sampler/Pad.hpp"

#include <Mpc.hpp>

#include <lcdgui/Background.hpp>
#include <sampler/Program.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;

MetronomeSoundScreen::MetronomeSoundScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "metronome-sound", layerIndex)
{
	sampler = mpc.getSampler();
}

void MetronomeSoundScreen::open()
{
    setSound(sound);
    mpc.addObserver(this); // Subscribe to "note" messages
}

void MetronomeSoundScreen::close()
{
    mpc.deleteObserver(this);
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
    auto note = dynamic_cast<Program*>(sampler->getProgram(program).get())->getPad(accentPad)->getNote();
	findField("accent")->setText((note == 34 ? "--" : std::to_string(note)) + "/" + sampler->getPadName(accentPad));
}

void MetronomeSoundScreen::displayNormal()
{
    auto program = sampler->getDrumBusProgramIndex(sound);
    auto note = dynamic_cast<Program*>(sampler->getProgram(program).get())->getPad(normalPad)->getNote();
    findField("normal")->setText((note == 34 ? "--" : std::to_string(note)) + "/" + sampler->getPadName(normalPad));
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
    if (i < 0 || i > 100)
        return;

    volume = i;
    displayVolume();
}

int MetronomeSoundScreen::getOutput()
{
    return output;
}

void MetronomeSoundScreen::setOutput(int i)
{
    if (i < 0 || i > 8)
        return;

    output = i;
    displayOutput();
}

int MetronomeSoundScreen::getSound()
{
    return sound;
}

void MetronomeSoundScreen::setSound(int i)
{
    if (i < 0 || i > 4)
        return;

    sound = i;
    displaySound();
    
    if (sound == 0)
    {
        displayVolume();
        displayOutput();
        findBackground()->setName("metronome-sound");
    }
    else
    {
        displayAccent();
        displayNormal();
        displayAccentVelo();
        displayNormalVelo();
        findBackground()->setName("metronome-sound-empty");
    }
}

int MetronomeSoundScreen::getAccentPad()
{
    return accentPad;
}

void MetronomeSoundScreen::setAccentPad(int i)
{
    if (i < 0 || i > 63)
        return;

    accentPad = i;
    displayAccent();
}

int MetronomeSoundScreen::getAccentVelo()
{
    return accentVelo;
}

void MetronomeSoundScreen::setAccentVelo(int i)
{
    if (i < 1 || i > 127)
        return;

    accentVelo = i;
    displayAccentVelo();
}

int MetronomeSoundScreen::getNormalPad()
{
    return normalPad;
}

void MetronomeSoundScreen::setNormalPad(int i)
{
    if (i < 0 || i > 63)
        return;

    normalPad = i;
    displayNormal();
}

int MetronomeSoundScreen::getNormalVelo()
{
    return normalVelo;
}

void MetronomeSoundScreen::setNormalVelo(int i)
{
    if (i < 1 || i > 127)
        return;

    normalVelo = i;
    displayNormalVelo();
}

void MetronomeSoundScreen::function(int i)
{
    init();

    switch (i)
    {
    case 3:
        openScreen("count-metronome");
        break;
    }
}

void MetronomeSoundScreen::turnWheel(int i)
{
    init();
    
    if (param == "sound") {
        setSound(sound + i);
    }
    else if (param == "volume") {
        setVolume(volume + i);
    }
    else if (param == "output") {
        setOutput(output + i);
    }
    else if (param == "accent") {
        setAccentPad(accentPad + i);
    }
    else if (param == "normal") {
        setNormalPad(normalPad + i);
    }
    else if (param == "velocity-accent") {
        setAccentVelo(accentVelo + i);
    }
    else if (param == "velocity-normal") {
        setNormalVelo(normalVelo + i);
    }
}

void MetronomeSoundScreen::update(Observable* o, Message message)
{
    init();

    const auto msg = std::get<std::string>(message);

    if (msg == "note")
    {
        if (param == "accent")
            setAccentPad(mpc.getPad());
        else if (param == "normal")
            setNormalPad(mpc.getPad());
    }
}
