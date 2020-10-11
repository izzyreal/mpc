#include "MetronomeSoundScreen.hpp"

#include <Mpc.hpp>

#include <lcdgui/Background.hpp>
#include <lcdgui/Field.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace mpc::sampler;
using namespace std;

MetronomeSoundScreen::MetronomeSoundScreen(mpc::Mpc& mpc, const int layerIndex)
	: ScreenComponent(mpc, "metronome-sound", layerIndex)
{
	sampler = mpc.getSampler();
}

void MetronomeSoundScreen::open()
{
    setSound(sound);
    mpc.addObserver(this); // Subscribe to "padandnote" messages
}

void MetronomeSoundScreen::close()
{
    mpc.deleteObserver(this);
}

void MetronomeSoundScreen::displaySound()
{
    findField("sound").lock()->setText(soundNames[sound]);
    findLabel("volume").lock()->Hide(sound != 0);
    findField("volume").lock()->Hide(sound != 0);
    findLabel("output").lock()->Hide(sound != 0);
    findField("output").lock()->Hide(sound != 0);
    findLabel("normal").lock()->Hide(sound == 0);
    findField("normal").lock()->Hide(sound == 0);
    findLabel("velocity-normal").lock()->Hide(sound == 0);
    findField("velocity-normal").lock()->Hide(sound == 0);
    findLabel("accent").lock()->Hide(sound == 0);
    findField("accent").lock()->Hide(sound == 0);
    findLabel("velocity-accent").lock()->Hide(sound == 0);
    findField("velocity-accent").lock()->Hide(sound == 0);
}

void MetronomeSoundScreen::displayVolume()
{
    findField("volume").lock()->setTextPadded(volume);
}

void MetronomeSoundScreen::displayOutput()
{
    auto outputText = output == 0 ? "STEREO" : "OUT" + to_string(output);
    findField("output").lock()->setText(outputText);
}

void MetronomeSoundScreen::displayAccent()
{
	auto program = sampler.lock()->getDrumBusProgramNumber(sound);
    auto note = dynamic_cast<Program*>(sampler.lock()->getProgram(program).lock().get())->getPad(accentPad)->getNote();
	findField("accent").lock()->setText((note == 34 ? "--" : to_string(note)) + "/" + sampler.lock()->getPadName(accentPad));
}

void MetronomeSoundScreen::displayNormal()
{
    auto program = sampler.lock()->getDrumBusProgramNumber(sound);
    auto note = dynamic_cast<Program*>(sampler.lock()->getProgram(program).lock().get())->getPad(normalPad)->getNote();
    findField("normal").lock()->setText((note == 34 ? "--" : to_string(note)) + "/" + sampler.lock()->getPadName(normalPad));
}

void MetronomeSoundScreen::displayAccentVelo()
{
    findField("velocity-accent").lock()->setTextPadded(accentVelo);
}

void MetronomeSoundScreen::displayNormalVelo()
{
    findField("velocity-normal").lock()->setTextPadded(normalVelo);
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
        findBackground().lock()->setName("metronome-sound");
    }
    else
    {
        displayAccent();
        displayNormal();
        displayAccentVelo();
        displayNormalVelo();
        findBackground().lock()->setName("metronome-sound-empty");
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
    if (i < 0 || i > 127)
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
    if (i < 0 || i > 127)
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
    
    if (param.compare("sound") == 0) {
        setSound(sound + i);
    }
    else if (param.compare("volume") == 0) {
        setVolume(volume + i);
    }
    else if (param.compare("output") == 0) {
        setOutput(output + i);
    }
    else if (param.compare("accent") == 0) {
        setAccentPad(accentPad + i);
    }
    else if (param.compare("normal") == 0) {
        setNormalPad(normalPad + i);
    }
    else if (param.compare("velocity-accent") == 0) {
        setAccentVelo(accentVelo + i);
    }
    else if (param.compare("velocity-normal") == 0) {
        setNormalVelo(normalVelo + i);
    }
}

void MetronomeSoundScreen::update(moduru::observer::Observable* o, nonstd::any msg)
{
    init();
    auto s = nonstd::any_cast<string>(msg);

    if (s.compare("padandnote") == 0)
    {
        if (param.compare("accent") == 0)
            setAccentPad(mpc.getPad());
        else if (param.compare("normal") == 0)
            setNormalPad(mpc.getPad());
    }
}
