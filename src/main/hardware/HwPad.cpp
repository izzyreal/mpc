#include <hardware/HwPad.hpp>

#include <Mpc.hpp>

#include <controls/BaseControls.hpp>
#include <controls/GlobalReleaseControls.hpp>

using namespace mpc::hardware;
using namespace std;

HwPad::HwPad(mpc::Mpc& mpc, int index)
	: mpc(mpc)
{
	this->index = index;
}

int HwPad::getIndex() {
	return index;
}

void HwPad::push(int velo) {
	auto c = mpc.getActiveControls();
	
	if (!c) return;
	
	c->pad(index, velo, false, 0);
}

void HwPad::release() {
	auto c = mpc.getReleaseControls();
	auto controls = mpc.getControls();
	if (!c) return;
	c->simplePad(index);
}
