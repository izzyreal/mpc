#include "StartFineScreen.hpp"
#include "hardware/Hardware.hpp"

#include "lcdgui/screens/TrimScreen.hpp"

using namespace mpc::lcdgui;
using namespace mpc::lcdgui::screens;
using namespace mpc::lcdgui::screens::window;

StartFineScreen::StartFineScreen(mpc::Mpc &mpc, const int layerIndex)
    : ScreenComponent(mpc, "start-fine", layerIndex)
{
    addChildT<Wave>()->setFine(true);
}

void StartFineScreen::open()
{
    findField("smpllngth")->setAlignment(Alignment::Centered);
    displayStart();
    findField("start")->enableTwoDots();
    displayLngthLabel();
    findLabel("lngth")->enableTwoDots();
    displaySmplLngth();
    displayPlayX();
    displayFineWave();
}

void StartFineScreen::displayFineWave()
{
    auto trimScreen = mpc.screens->get<TrimScreen>();

    auto sound = sampler->getSound();

    if (!sound)
    {
        return;
    }

    findWave()->setSampleData(sound->getSampleData(), sound->isMono(),
                              trimScreen->view);
    findWave()->setCenterSamplePos(sound->getStart());
}

void StartFineScreen::displayStart()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        return;
    }

    findField("start")->setTextPadded(sound->getStart(), " ");
}

void StartFineScreen::displayLngthLabel()
{
    auto sound = sampler->getSound();

    if (!sound)
    {
        return;
    }

    findLabel("lngth")->setTextPadded(sound->getEnd() - sound->getStart(), " ");
}

void StartFineScreen::displaySmplLngth()
{
    auto trimScreen = mpc.screens->get<TrimScreen>();
    findField("smpllngth")->setText(trimScreen->smplLngthFix ? "FIX" : "VARI");
}

void StartFineScreen::displayPlayX()
{
    findField("playx")->setText(playXNames[sampler->getPlayX()]);
}

void StartFineScreen::function(int i)
{

    ScreenComponent::function(i);

    switch (i)
    {
        case 1:
            findWave()->zoomPlus();
            break;
        case 2:
            findWave()->zoomMinus();
            break;
        case 4:
            sampler->playX();
            break;
    }
}

void StartFineScreen::turnWheel(int i)
{
    auto sound = sampler->getSound();
    auto startEndLength = static_cast<int>(sound->getEnd() - sound->getStart());
    auto trimScreen = mpc.screens->get<TrimScreen>();

    auto sampleLength = sound->getFrameCount();
    auto soundInc = getSoundIncrement(i);

    auto focusedField = getFocusedFieldOrThrow();

    if (focusedField->isSplit())
    {
        soundInc = focusedField->getSplitIncrement(i >= 0);
    }

    if (focusedField->isTypeModeEnabled())
    {
        focusedField->disableTypeMode();
    }

    const auto focusedFieldName = focusedField->getName();

    if (focusedFieldName == "start")
    {
        auto highestStartPos = sampleLength - 1;
        auto length = sound->getEnd() - sound->getStart();

        if (trimScreen->smplLngthFix)
        {
            highestStartPos -= startEndLength;

            if (sound->getStart() + soundInc > highestStartPos)
            {
                return;
            }
        }

        sound->setStart(sound->getStart() + soundInc);

        if (trimScreen->smplLngthFix)
        {
            sound->setEnd(sound->getStart() + length);
        }

        displayFineWave();
        displayLngthLabel();
        displayStart();
    }
    else if (focusedFieldName == "smpllngth")
    {
        trimScreen->smplLngthFix = i > 0;
        displaySmplLngth();
    }
    else if (focusedFieldName == "playx")
    {
        sampler->setPlayX(sampler->getPlayX() + i);
        displayPlayX();
    }
}

void StartFineScreen::left()
{
    command::SplitLeftCommand(mpc).execute();
}

void StartFineScreen::right()
{
    command::SplitRightCommand(mpc).execute();
}

void StartFineScreen::pressEnter()
{
    auto trimScreen = mpc.screens->get<TrimScreen>();
    trimScreen->pressEnter();
    displayStart();
    displayLngthLabel();
    displayFineWave();
}

void StartFineScreen::setSlider(int i)
{
    if (!mpc.getHardware()
             ->getButton(hardware::ComponentId::SHIFT)
             ->isPressed())
    {
        return;
    }

    const auto focusedFieldName = getFocusedFieldNameOrThrow();

    if (focusedFieldName == "start")
    {
        auto trimScreen = mpc.screens->get<TrimScreen>();
        trimScreen->setSliderStart(i);
        displayStart();
        displayLngthLabel();
        displayFineWave();
    }
}
