#include "DataWheel.hpp"

#include <Mpc.hpp>
#include <lcdgui/ScreenComponent.hpp>
#include <lcdgui/screens/OpensNameScreen.hpp>
#include <disk/AbstractDisk.hpp>

using namespace mpc::hardware;
using namespace mpc::lcdgui::screens;

DataWheel::DataWheel(mpc::Mpc& mpc)
	: mpc(mpc)
{
}

void DataWheel::turn(int increment)
{
	auto controls = mpc.getActiveControls();
	
    if (controls)
	{
		controls->turnWheel(increment);

		if (auto opensNameScreen = std::dynamic_pointer_cast<OpensNameScreen>(controls))
		{
			opensNameScreen->openNameScreen();
		}
	}

	updateUi(increment);
}

void DataWheelUp::push(int increment)
{
    dataWheel.turn(increment);
}

void DataWheelDown::push(int decrement)
{
    dataWheel.turn(-decrement);
}
