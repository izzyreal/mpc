#include "audiomidi/AudioMidiServices.hpp"
#include "engine/PreviewSoundPlayer.hpp"
#include "lcdgui/screens/DrumScreen.hpp"
#include "lcdgui/screens/window/StepEditOptionsScreen.hpp"
#include "lcdgui/screens/window/TimingCorrectScreen.hpp"
#include <hardware/HwPad.hpp>

#include <Mpc.hpp>

#include <lcdgui/screens/OpensNameScreen.hpp>
#include <lcdgui/ScreenComponent.hpp>
#include <controls/GlobalReleaseControls.hpp>

#include "controller/PadContextFactory.h"

using namespace mpc::hardware;
using namespace mpc::lcdgui::screens;

HwPad::HwPad(mpc::Mpc& mpc, int index)
        : HwComponent(mpc, "pad-" + std::to_string(index + 1))
{
    this->index = index;
}

int HwPad::getIndex()
{
    return index;
}

void HwPad::push(int velo)
{
    if (auto opensNameScreen = std::dynamic_pointer_cast<OpensNameScreen>(mpc.getActiveControls()))
    {
        opensNameScreen->openNameScreen();
    }

    HwComponent::push(velo);

    if (isPressed())
    {
        return;
    }

    pressure = velo;

    padIndexWithBankWhenLastPressed = index + (mpc.getBank() * 16);

    auto c = mpc.getActiveControls();
    if (!c)
        return;
    c->pad(padIndexWithBankWhenLastPressed, velo);
}

void HwPad::release()
{
    if (!isPressed())
    {
        return;
    }

    pressure = 0;

    auto c = mpc.getReleaseControls();

    if (!c || padIndexWithBankWhenLastPressed == -1)
    {
        padIndexWithBankWhenLastPressed = -1;
        return;
    }

    const auto currentScreenName = mpc.getLayeredScreen()->getCurrentScreenName();

    auto ctx = controller::PadContextFactory::buildPadReleaseContext(mpc, padIndexWithBankWhenLastPressed, currentScreenName);

    controls::GlobalReleaseControls::simplePad(ctx);

    padIndexWithBankWhenLastPressed = -1;
}

bool HwPad::isPressed()
{
    return pressure > 0;
}

void HwPad::setPressure(unsigned char newPressure)
{
    pressure = newPressure;
}

char HwPad::getPadIndexWithBankWhenLastPressed()
{
    return padIndexWithBankWhenLastPressed;
}

unsigned char HwPad::getPressure()
{
    return pressure;
}

void HwPad::setPadIndexWithBankWhenLastPressed(char padIndexWithBank)
{
  padIndexWithBankWhenLastPressed = padIndexWithBank;
}
