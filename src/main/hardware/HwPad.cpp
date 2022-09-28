#include <hardware/HwPad.hpp>

#include <Mpc.hpp>

#include <lcdgui/ScreenComponent.hpp>
#include <controls/GlobalReleaseControls.hpp>

using namespace mpc::hardware;
using namespace std;

HwPad::HwPad(mpc::Mpc& mpc, int index)
        : HwComponent(mpc, "pad-" + to_string(index + 1))
{
    this->index = index;
}

int HwPad::getIndex()
{
    return index;
}

void HwPad::push(int velo)
{
    if (isPressed())
    {
        return;
    }

    pressure = velo;

    padIndexWithBankWhenLastPressed = index + (mpc.getBank() * 16);

    auto c = mpc.getActiveControls().lock();

    if (!c)
        return;

    c->pad(padIndexWithBankWhenLastPressed, velo, false, 0);
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

    c->simplePad(padIndexWithBankWhenLastPressed);

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

void HwPad::setPadIndexWithBankWhenLastPressed(char padIndexWithBank)
{
  padIndexWithBankWhenLastPressed = padIndexWithBank;
}
