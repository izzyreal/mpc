#include <hardware/HwPad.hpp>

#include <Mpc.hpp>

#include <lcdgui/ScreenComponent.hpp>
#include <controls/GlobalReleaseControls.hpp>

using namespace mpc::hardware;
using namespace std;

HwPad::HwPad(mpc::Mpc& mpc, int index)
	: mpc(mpc)
{
	this->index = index;
}

int HwPad::getIndex()
{
	return index;
}

void HwPad::push(int velo)
{
	auto c = mpc.getActiveControls().lock();
	
	if (!c)
		return;
	
	c->pad(index, velo, false, 0);
}

void HwPad::release()
{
	auto c = mpc.getReleaseControls();

	if (!c)
		return;

	c->simplePad(index);
}
