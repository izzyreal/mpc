#include "HwSlider.hpp"

#include <Mpc.hpp>
#include <lcdgui/ScreenComponent.hpp>

using namespace mpc::hardware;

Slider::Slider(mpc::Mpc& mpc)
	: mpc(mpc)
{
}

void Slider::setValue(int i)
{
	if (i < 0 || i > 127)
		return;

	value = i;

    auto activeControls = mpc.getActiveControls();

    if (activeControls)
    {
        activeControls->setSlider(value);
    }
}

int Slider::getValue() {
	return value;
}
