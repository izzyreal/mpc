#include "MetronomeSoundScreen.hpp"

#include <Mpc.hpp>
#include <ui/Uis.hpp>
#include <lcdgui/Background.hpp>
#include <lcdgui/Field.hpp>
#include <sampler/Program.hpp>
#include <sampler/Sampler.hpp>

using namespace mpc::lcdgui::screens::dialog;
using namespace std;

MetronomeSoundScreen::MetronomeSoundScreen(const int layer)
	: ScreenComponent("metronomesound", layer)
{
	sampler = Mpc::instance().getSampler();
}

void MetronomeSoundScreen::open()
{
    displaySound();

    if (metronomeSound == 0)
    {
        displayVolume();
        displayOutput();
    }
    else
    {
        displayAccent();
        displayNormal();
        displayAccentVelo();
        displayNormalVelo();
        findBackground().lock()->setName("metronomesoundempty");
    }
}

void MetronomeSoundScreen::displaySound()
{
    findField("sound").lock()->setText(soundNames[metronomeSound]);
	/*
    volumeLabel->setVisible(sound == 0);
    volumeField->setVisible(sound == 0);
    outputLabel->setVisible(sound == 0);
    outputField->setVisible(sound == 0);
    normalLabel->setVisible(sound > 0);
    normalField->setVisible(sound > 0);
    veloNormalLabel->setVisible(sound > 0);
    veloNormalField->setVisible(sound > 0);
	accentLabel->setVisible(sound > 0);
    accentField->setVisible(sound > 0);
    veloAccentLabel->setVisible(sound > 0);
    veloAccentField->setVisible(sound > 0);
	*/
}

void MetronomeSoundScreen::displayVolume()
{
    //volumeField.lock()->setText(StringUtils::rightPad(swGui)->getClickVolume()), 3));
}

void MetronomeSoundScreen::displayOutput()
{
    findField("output").lock()->setText(clickOutput == 0 ? "STEREO" : to_string(clickOutput));
}

void MetronomeSoundScreen::displayAccent()
{
	auto lSampler = sampler.lock();
	auto program = lSampler->getDrumBusProgramNumber(metronomeSound);
    auto pad = lSampler->getProgram(program).lock()->getPadNumberFromNote(accentNote);
	findField("accent").lock()->setText(accentNote + "/" + lSampler->getPadName(pad));
}

void MetronomeSoundScreen::displayNormal()
{
	auto lSampler = sampler.lock();
	auto program = lSampler->getDrumBusProgramNumber(metronomeSound);
    auto pad = lSampler->getProgram(program).lock()->getPadNumberFromNote(normalNote);
	findField("normal").lock()->setText(to_string(normalNote) + "/" + lSampler->getPadName(pad));
}

void MetronomeSoundScreen::displayAccentVelo()
{
    //veloAccentField.lock()->setText(rightPad(to_string(AccentVelo()), 3));
}

void MetronomeSoundScreen::displayNormalVelo()
{
    //veloNormalField.lock()->setText(rightPad(to_string(NormalVelo()), 3));
}

int MetronomeSoundScreen::getClickVolume()
{
    return clickVolume;
}

void MetronomeSoundScreen::setClickVolume(int i)
{
    if (i < 0 || i > 100)
    {
        return;
    }

    clickVolume = i;
    displayVolume();
}

int MetronomeSoundScreen::getClickOutput()
{
    return clickOutput;
}

void MetronomeSoundScreen::setClickOutput(int i)
{
    if (i < 0 || i > 8)
    {
        return;
    }

    clickOutput = i;
    displayOutput();
}

int MetronomeSoundScreen::getMetronomeSound()
{
    return metronomeSound;
}

void MetronomeSoundScreen::setMetronomeSound(int i)
{
    if (i < 0 || i > 4)
    {
        return;
    }

    metronomeSound = i;
    displaySound();
}

int MetronomeSoundScreen::getAccentNote()
{
    return accentNote;
}

void MetronomeSoundScreen::setAccentNote(int i)
{
    if (i < 35 || i > 98)
    {
        return;
    }

    accentNote = i;
    displayAccent();
}

int MetronomeSoundScreen::getAccentVelo()
{
    return accentVelo;
}

void MetronomeSoundScreen::setAccentVelo(int i)
{
    if (i < 0 || i > 127)
    {
        return;
    }

    accentVelo = i;
    displayAccentVelo();
}

int MetronomeSoundScreen::getNormalNote()
{
    return normalNote;
}

void MetronomeSoundScreen::setNormalNote(int i)
{
    if (i < 35 || i > 98)
    {
        return;
    }

    normalNote = i;
    displayNormal();
}

int MetronomeSoundScreen::getNormalVelo()
{
    return normalVelo;
}

void MetronomeSoundScreen::setNormalVelo(int i)
{
    if (i < 0 || i > 127)
    {
        return;
    }

    normalVelo = i;
    displayNormalVelo();
}

void MetronomeSoundScreen::function(int i)
{
    init();
    switch (i) {
    case 3:
        ls.lock()->openScreen("countmetronome");
        break;
    }
}

void MetronomeSoundScreen::turnWheel(int i)
{
    init();
    if (param.compare("sound") == 0) {
        setMetronomeSound(metronomeSound + i);
    }
    else if (param.compare("volume") == 0) {
        setClickVolume(clickVolume + i);
    }
    else if (param.compare("output") == 0) {
        setClickOutput(clickOutput + i);
    }
    else if (param.compare("accent") == 0) {
        setAccentNote(accentNote + i);
    }
    else if (param.compare("normal") == 0) {
        setNormalNote(normalNote + i);
    }
    else if (param.compare("velocityaccent") == 0) {
        setAccentVelo(accentVelo + i);
    }
    else if (param.compare("velocitynormal") == 0) {
        setNormalVelo(normalVelo + i);
    }
}
