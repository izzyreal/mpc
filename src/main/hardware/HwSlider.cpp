#include "HwSlider.hpp"

#include <Mpc.hpp>
#include <controls/BaseControls.hpp>

using namespace mpc::hardware;
using namespace std;

Slider::Slider()
{
	
}

void Slider::setValue(int i) {
	if (i < 0 || i > 127) return;
	value = i;
	if (Mpc::instance().getActiveControls() != nullptr) {
		Mpc::instance().getActiveControls()->setSlider(value);
	}
}

int Slider::getValue() {
	return value;
}

Slider::~Slider() {
}
