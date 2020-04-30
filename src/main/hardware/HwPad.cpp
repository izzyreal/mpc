#include <hardware/HwPad.hpp>

#include <Mpc.hpp>

#include <controls/BaseControls.hpp>
#include <controls/GlobalReleaseControls.hpp>

using namespace mpc::hardware;
using namespace std;

HwPad::HwPad(int index)
{
	
	this->index = index;
}

int HwPad::getIndex() {
	return index;
}

void HwPad::push(int velo) {
	auto c = Mpc::instance().getActiveControls();
	if (!c) return;
	c->pad(index, velo, false, 0);
}

void HwPad::release() {
	auto c = Mpc::instance().getReleaseControls();
	auto controls = Mpc::instance().getControls();
	if (!c) return;
	c->simplePad(index);
}

HwPad::~HwPad() {
}
