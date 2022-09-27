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
  down = true;
  pressure = velo;
  
	auto c = mpc.getActiveControls().lock();
	
	if (!c)
		return;
	
	c->pad(index, velo, false, 0);
}

void HwPad::release()
{
  down = false;
  pressure = 0;
  
	auto c = mpc.getReleaseControls();

	if (!c)
		return;

	c->simplePad(index);
}

bool HwPad::isDown()
{
  return down;
}

unsigned char HwPad::getPressure()
{
  return pressure;
}

void HwPad::setPressure(unsigned char newPressure)
{
  if (newPressure < 0 || newPressure > 127)
  {
    return;
  }
  
  pressure = newPressure;
}
